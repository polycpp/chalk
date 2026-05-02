# Divergences From Upstream

## Deferred Features

- A `createSupportsColor`/`detectColorSupport` overload that takes an explicit `sniffFlags`-style argument. The current C++ implementation reads `--color`/`--no-color` solely through the env-variable ladder; explicit CLI override is deferred until a real consumer needs it.

## Deliberate Behavior Changes

- `Chalk` chain methods are explicit member functions (`chalk.red().bold()(...)`) rather than JavaScript-style getter chains (`chalk.red.bold(...)`). C++ has no equivalent of JavaScript's `Object.defineProperty` getter chain on a callable function, so the chain is exposed as method calls.
- `Chalk::level()` and `Chalk::setLevel(int)` replace the writable `chalk.level` data property to avoid exposing a public mutable field.
- `Chalk` chains share the parent's level cell via `std::shared_ptr<int>`, so calling `setLevel` on a parent is observed lazily by every chain previously built from it (see `ChildChain*` tests in `tests/test_chalk.cpp`). This matches upstream JS chalk's prototype-getter `level` semantics in `test/level.js`.
- Multi-argument `Chalk::operator()` accepts arithmetic types (formatted via `std::to_string`), `const char*` / `char*`, and types that convert to `std::string`. JavaScript's loose coercion of arrays/objects/booleans is not replicated; callers should pre-format complex values themselves.
- Invalid hex strings for `hex()` / `bgHex()` collapse to `(0, 0, 0)`; chalk does not throw on invalid hex. The C++ port intentionally accepts a stricter subset than upstream's `/[a-f\d]{6}|[a-f\d]{3}/i.exec(...)` regex: it walks the input from the start (skipping a single leading `#`) and breaks on the first non-hex non-`#` character, then requires the collected run to be exactly 3 or 6 hex digits. Compared with upstream this differs in two cases:
  - 4-, 5-, 7-, or 8-digit pure hex runs collapse to `(0, 0, 0)`. Upstream's regex picks the longest match (6 first, then 3) anywhere in the string, so `"abcd"` returns the rgb of `"abc"` (170, 187, 204) upstream but `(0, 0, 0)` here.
  - Inputs with a non-`#` non-hex prefix collapse to `(0, 0, 0)`. Upstream skips ahead to the first 3- or 6-digit hex run, so `" #abc"` upstream returns the rgb of `"abc"` but `(0, 0, 0)` here.
  - This is an explicit deliberate departure: the C++ API accepts only well-formed `#RRGGBB` / `RRGGBB` / `#RGB` / `RGB` color literals plus an outright-invalid fallback, and does not promise upstream's broader "find any hex run anywhere in the string" lenience.
- `Options::level` validation throws `polycpp::Error` rather than a JavaScript `TypeError`; this matches the error style of other companion libraries.

## Audit findings (libgen catch-up)

Findings raised during the 2026-05-01 catch-up review. AF-A through AF-F
were resolved on 2026-05-02; AF-G is intentionally tracked at the libgen
template level instead of in this repo. Original descriptions kept
verbatim for traceability; resolution notes added inline.

| ID | Severity | Location | Description | Resolution |
|---|---|---|---|---|
| AF-2026-05-01-A | low | `include/polycpp/chalk/detail/supports_color.hpp:13-18`, `:122-123` | The companion uses a private `#ifdef _WIN32 ... isatty_impl` macro instead of the now-available `polycpp::platform::isTerminal(int fd)` from `<polycpp/platform/console.hpp>`. The companion predates that header. | resolved 2026-05-02 — switched to `polycpp::platform::isTerminal`; the conditional `<unistd.h>`/`<io.h>` include and the `isatty_impl` macro are gone. Behavior is identical. |
| AF-2026-05-01-B | medium | `include/polycpp/chalk/detail/chalk.hpp:71` (`Chalk::Chalk()`) and `:73-80` (`Chalk(const Options&)` when `options.level` is unset) | Both default-constructor and `Options{}` (with `level` unset) initialise `level_` to `0`. Upstream `applyOptions` falls back to `stdoutColor.level` from `supportsColor` when `options.level` is `undefined`. Global `chalk()` and `chalkStderr()` already auto-detect, but a user calling `Chalk()` or `Chalk(Options{})` directly gets a level-0 instance instead of the auto-detected level. | resolved 2026-05-02 — `Chalk()` and `Chalk(Options{})` now fall back to `supportsColor().level` for parity. Pinned by `ChalkTest.DefaultConstructionAutoDetectsLevel` and `ChalkTest.EmptyOptionsAutoDetectsLevel`. |
| AF-2026-05-01-C | medium | `include/polycpp/chalk/detail/chalk.hpp:93-106` (`withStyle`) | Chain links capture the parent's `level_` at creation time. Upstream JS reads `chalk.level` lazily from the prototype chain (see `test/level.js` `propagate enable/disable changes from child colors` — `red.level` reads `chalk.level` after the parent mutates). Mutating the parent's level after a chain has been built does not retroactively propagate in C++. | resolved 2026-05-02 — `Chalk` now stores `std::shared_ptr<int> level_ptr_` instead of `int level_`. Chain links share the cell, so `setLevel` on a parent is observed lazily by every chain previously built from it. Pinned by three new `ChildChain*` tests in `tests/test_chalk.cpp`. |
| AF-2026-05-01-D | low | `include/polycpp/chalk/detail/ansi_styles.hpp:48-92` (`hexToRgb`) | Upstream regex `/[a-f\d]{6}|[a-f\d]{3}/i.exec(hex.toString(16))` finds the first 3 OR 6 hex digits anywhere in the input and uses the longest-first alternation. The C++ port walks from the start, skipping `#`, and breaks on the first non-hex non-`#` character; a 4- or 5-digit hex run yields `(0, 0, 0)` instead of upstream's first-3 fallback, and a leading non-`#` character such as a space is treated as invalid instead of being skipped. | resolved 2026-05-02 — kept the stricter behavior, promoted to a deliberate behavior change in `## Deliberate Behavior Changes` with explicit edge cases. Pinned by four new `HexToRgb*` edge-case tests. |
| AF-2026-05-01-E | low | `tests/test_chalk.cpp` | The local GoogleTest suite does not cover the captured-level propagation case from `test/level.js` (lines 17-30) or the `Chalk()` / `Chalk(Options{})` auto-detect fallback case from upstream. AF-B and AF-C are uncovered by tests, so they are easy to regress silently. | resolved 2026-05-02 — eight new regression tests added: `DefaultConstructionAutoDetectsLevel`, `EmptyOptionsAutoDetectsLevel`, `ChildChainObservesParentLevelChangeRedThenZero`, `ChildChainObservesParentLevelChangeZeroThenThree`, `ChildChainPropagatesThroughDeepChain`, `HexToRgbFourDigitRunReturnsBlack`, `HexToRgbFiveDigitRunReturnsBlack`, `HexToRgbLeadingNonHashNonHexReturnsBlack`, `HexToRgbTrailingNonHexBreaksOnSeparator`. |
| AF-2026-05-01-F | low | `tests/test_chalk.cpp`, `_build_and_test.sh` | Tests were not re-run during this catch-up because the helper at `/data/work/lib/_build_and_test.sh` requires a populated `_deps` cache under `/data/repo/polycpp/build-clang14-gap-debug/_deps/`, which is not present in this environment. The pre-existing `build.build.log` shows `[581/581] Linking CXX executable test_chalk` from a prior local run, but that artifact was not produced by this catch-up. | resolved 2026-05-02 — built against `/data/repo/polycpp-old` with `_deps` from the same checkout; ran ctest. All 89 tests pass (81 pre-existing + 8 new). Validation command captured in `docs/test-plan.md ## Current validation`. |
| AF-2026-05-01-G | low | `CMakeLists.txt:12-17` | The `polycpp` `FetchContent_Declare` block does not honor a `POLYCPP_SOURCE_DIR` override (see `libgen/docs/cmake-dependency-patterns.md` baseline). Builds against a local in-tree polycpp checkout therefore go through the SSH-only Git remote even when a local source path is available. | tracked at libgen — the libgen baseline that prescribes `POLYCPP_SOURCE_DIR` is being removed, so this companion does not need the block. Local builds use `-DFETCHCONTENT_SOURCE_DIR_POLYCPP=...` which already works. |

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
