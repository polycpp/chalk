# Divergences From Upstream

## Deferred Features

- `os.release()`-based old Windows 10 truecolor detection. Modern Windows Terminals still set `WT_SESSION` and `COLORTERM=truecolor`, which the existing detection ladder honors, so the rare legacy path is left for a future revision.
- A `createSupportsColor`/`detectColorSupport` overload that takes an explicit `sniffFlags`-style argument. The current C++ implementation reads `--color`/`--no-color` solely through the env-variable ladder; explicit CLI override is deferred until a real consumer needs it.

## Deliberate Behavior Changes

- `Chalk` chain methods are explicit member functions (`chalk.red().bold()(...)`) rather than JavaScript-style getter chains (`chalk.red.bold(...)`). C++ has no equivalent of JavaScript's `Object.defineProperty` getter chain on a callable function, so the chain is exposed as method calls.
- `Chalk::level()` and `Chalk::setLevel(int)` replace the writable `chalk.level` data property to avoid exposing a public mutable field.
- `Chalk` chains do not retroactively pick up level changes on the parent. The level captured at chain creation is what the chain renders. JavaScript chalk reads `level` lazily from the prototype chain, but in C++ this would require a back-pointer that is unnecessary for the typical "build a theme, then call it" usage pattern. Tests in `tests/test_chalk.cpp` cover both the parent-then-child and child-then-parent ordering.
- Multi-argument `Chalk::operator()` accepts arithmetic types (formatted via `std::to_string`), `const char*` / `char*`, and types that convert to `std::string`. JavaScript's loose coercion of arrays/objects/booleans is not replicated; callers should pre-format complex values themselves.
- Invalid hex strings for `hex()` / `bgHex()` collapse to `(0, 0, 0)` (matching upstream lenient behavior); chalk does not throw on invalid hex.
- `Options::level` validation throws `polycpp::Error` rather than a JavaScript `TypeError`; this matches the error style of other companion libraries.

## Audit findings (libgen catch-up)

These are gaps surfaced while auditing the existing implementation against
upstream behavior and the libgen reuse rules during the 2026-05-01
catch-up review. They are NOT being fixed in this run; they are listed
here so triage can classify each one as a deferred behavior change, an
omitted feature, or a bug to fix in a follow-up.

| ID | Severity | Location | Description | Recommended classification |
|---|---|---|---|---|
| AF-2026-05-01-A | low | `include/polycpp/chalk/detail/supports_color.hpp:13-18`, `:122-123` | The companion uses a private `#ifdef _WIN32 ... isatty_impl` macro instead of the now-available `polycpp::platform::isTerminal(int fd)` from `<polycpp/platform/console.hpp>`. The companion predates that header. | refactor follow-up — switch to `polycpp::platform::isTerminal`; behavior is identical, only avoids duplicating cross-platform plumbing |
| AF-2026-05-01-B | medium | `include/polycpp/chalk/detail/chalk.hpp:71` (`Chalk::Chalk()`) and `:73-80` (`Chalk(const Options&)` when `options.level` is unset) | Both default-constructor and `Options{}` (with `level` unset) initialise `level_` to `0`. Upstream `applyOptions` falls back to `stdoutColor.level` from `supportsColor` when `options.level` is `undefined`. Global `chalk()` and `chalkStderr()` already auto-detect, but a user calling `Chalk()` or `Chalk(Options{})` directly gets a level-0 instance instead of the auto-detected level. | deferred (compatibility adjustment) — change the public `Chalk()` and `Chalk(Options{})` paths to fall back to `supportsColor().level` for parity, or document explicitly as a deliberate behavior change |
| AF-2026-05-01-C | medium | `include/polycpp/chalk/detail/chalk.hpp:93-106` (`withStyle`) | Chain links capture the parent's `level_` at creation time. Upstream JS reads `chalk.level` lazily from the prototype chain (see `test/level.js` `propagate enable/disable changes from child colors` — `red.level` reads `chalk.level` after the parent mutates). Mutating the parent's level after a chain has been built does not retroactively propagate in C++. | deliberate behavior change — already documented in `## Deliberate Behavior Changes`; consider adding a regression test covering the captured-level case so future readers see the divergence is intentional |
| AF-2026-05-01-D | low | `include/polycpp/chalk/detail/ansi_styles.hpp:48-92` (`hexToRgb`) | Upstream regex `/[a-f\d]{6}|[a-f\d]{3}/i.exec(hex.toString(16))` finds the first 3 OR 6 hex digits anywhere in the input and uses the longest-first alternation. The C++ port walks from the start, skipping `#`, and breaks on the first non-hex non-`#` character; a 4- or 5-digit hex run yields `(0, 0, 0)` instead of upstream's first-3 fallback, and a leading non-`#` character such as a space is treated as invalid instead of being skipped. | deferred (compatibility adjustment) — port the regex semantics literally for byte-for-byte parity with upstream, or document the lenient subset as a deliberate behavior change |
| AF-2026-05-01-E | low | `tests/test_chalk.cpp` | The local GoogleTest suite does not cover the captured-level propagation case from `test/level.js` (lines 17-30) or the `Chalk()` / `Chalk(Options{})` auto-detect fallback case from upstream. AF-B and AF-C are uncovered by tests, so they are easy to regress silently. | follow-up — add a regression test once AF-B and AF-C decisions are landed |
| AF-2026-05-01-F | low | `tests/test_chalk.cpp`, `_build_and_test.sh` | Tests were not re-run during this catch-up because the helper at `/data/work/lib/_build_and_test.sh` requires a populated `_deps` cache under `/data/repo/polycpp/build-clang14-gap-debug/_deps/`, which is not present in this environment. The pre-existing `build.build.log` shows `[581/581] Linking CXX executable test_chalk` from a prior local run, but that artifact was not produced by this catch-up. | follow-up — re-run `_build_and_test.sh /data/work/lib/chalk` against a workstation with the polycpp deps cache; record the validation output in `docs/test-plan.md ## Current validation` |
| AF-2026-05-01-G | low | `CMakeLists.txt:12-17` | The `polycpp` `FetchContent_Declare` block does not honor a `POLYCPP_SOURCE_DIR` override (see `libgen/docs/cmake-dependency-patterns.md` baseline). Builds against a local in-tree polycpp checkout therefore go through the SSH-only Git remote even when a local source path is available. | refactor follow-up — add the `POLYCPP_SOURCE_DIR` override block from the libgen baseline so embedded builds and `_build_and_test.sh -DFETCHCONTENT_SOURCE_DIR_POLYCPP=...` both work without recreating the SSH remote |

No security regressions were found: chalk has no auth, crypto, parser
boundary, or protocol surface, and the analyzer reports
`securityHints.securitySensitive = false`.

## Unsupported Runtime-Specific Features

- JavaScript `Symbol('GENERATOR')`, `Symbol('STYLER')`, `Symbol('IS_EMPTY')` brand keys: replaced by private members on the `Chalk` class. Symbols are a JavaScript runtime construct without a C++ equivalent.
- `Object.setPrototypeOf` callable-instance trick that makes a chalk instance both a function and an object: replaced by C++ `operator()`.
- `Object.defineProperty` memoization on first chain access: not necessary in C++. Chain creation is a constant-time `shared_ptr<Styler>` link with no Symbol bookkeeping.
- ESM-only `imports` map (`#ansi-styles`, `#supports-color`): C++ resolves these vendored modules at compile time through `include/polycpp/chalk/ansi_styles.hpp` and `include/polycpp/chalk/supports_color.hpp`. The package-imports indirection is a JavaScript packaging detail.
- Browser `supports-color` stub (`source/vendor/supports-color/browser.js`): C++ has no browser target.
- The single analyzer-flagged Node parity signal (`callback-argument`) is not surfaced as a public callback API; the matched calls are JS-internal `Object.entries(...).forEach(...)`-style iteration, not part of chalk's contract.
