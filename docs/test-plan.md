# Test Plan

## Unit tests

- ANSI escape generation: `wrapAnsi16`, `wrapAnsi256` (foreground and background), `wrapAnsi16m` (foreground and background) emit the expected escape strings.
- RGB/256/hex math: `rgbToAnsi256` for both grayscale and color regions; `ansi256ToAnsi` for `0..7`, `8..15`, `16..231`, `232..255`; `hexToRgb` for 6-char, 3-char, lowercase, and invalid input; `rgbToAnsi` end-to-end.
- `makeColorCode` truecolor (level 3), 256 (level 2), 16 (level 1) for both foreground and background; `makeColorCode(level, hex)` overload; `makeColorCode256` always emits a 256-color sequence regardless of level.
- `Chalk` default construction and `Options{}` fall back to the auto-detected stdout level; `Options{level=N}` sets level N; out-of-range levels throw `polycpp::Error`; `setLevel` updates the level on an existing instance.
- Empty input string returns empty; level 0 returns plain text; no chain returns plain text.
- Modifier methods (reset/bold/dim/italic/underline/overline/inverse/hidden/strikethrough) emit the correct open/close codes.
- Foreground/background standard 16 + bright 16 + `gray`/`grey` aliases emit correct codes.
- `rgb`/`hex`/`ansi256`/`bgRgb`/`bgHex`/`bgAnsi256` produce expected escapes.
- `visible` returns plain text when level is 0 even with styles applied; passes through styles when level > 0; works as both `visible.red(...)` and `red.visible(...)`.
- Multi-arg `operator()` joins with `" "` and converts arithmetic and char-pointer arguments via `std::to_string` and direct construction.
- Modifier/foreground/background/color name arrays match upstream order and content.

## Integration tests

- Chained styles produce the upstream-exact output for `chalk.red().bgGreen().underline()("foo")` and other multi-style chains.
- Reusable theme: `auto error = chalk.bold().red(); error("nope")` matches `chalk.bold().red()("nope")`.
- Nested styles: `chalk.red("foo" + chalk.underline.bgBlue("bar") + "!")` rewrites the inner close codes to close+open and matches upstream output exactly.
- Multi-level nesting (`chalk.red('a' + chalk.yellow('b' + chalk.green('c') + 'b') + 'c')`) matches upstream byte-for-byte.
- CRLF encasing: line breaks close the active style, emit `\n` or `\r\n`, and reopen the active style.
- Color downsampling: at level 2, RGB/hex inputs produce 256-color sequences; at level 1, RGB/hex inputs produce 16-color sequences; at level 3, RGB/hex inputs produce truecolor sequences.
- `ansi256(...)` always emits a 256-color sequence even when level is 1.
- Global `chalk()` and `chalkStderr()` instances exist, are independently mutable, and start with the auto-detected level for fd 1 / fd 2.

## Compatibility tests adapted from upstream

- upstream compatibility layout: GoogleTest cases in `tests/test_chalk.cpp` adapted from upstream `test/*.js`. The existing test file groups assertions by suite (`AnsiStylesTest`, `ChalkTest`, `SupportsColorTest`, `NameArraysTest`, `GlobalInstanceTest`), so a separate `tests/upstream/` directory was not introduced for v0; the suite groupings already mirror upstream test files.
- upstream-to-local coverage map:
  - `test/chalk.js` -> `tests/test_chalk.cpp` `ChalkTest::*` (basic styling, multi-args, nesting, line breaks, multiple styles, falsy values, reset wrapping, JS nesting parity)
  - `test/visible.js` -> `tests/test_chalk.cpp` `ChalkTest::Visible*`
  - `test/level.js` -> `tests/test_chalk.cpp` `ChalkTest::Level*`, `ChalkTest::RgbDownsampleTo256`, `ChalkTest::RgbDownsampleTo16`, `ChalkTest::Ansi256NoDownsampleAtLevel1`
  - `test/instance.js` -> `tests/test_chalk.cpp` `GlobalInstanceTest::ChalkInstanceIsMutable` and `ChalkTest::OptionsLevel`
  - `test/no-color-support.js` -> `tests/test_chalk.cpp` `ChalkTest::Level0ReturnsPlainText` and `ChalkTest::JsNoCodesAtLevel0`
- omitted upstream cases:
  - `test/_fixture.js`: not applicable because it is an ava harness fixture
  - `source/index.test-d.ts`: not applicable because TypeScript-only type assertions cannot run in C++; the literal-union coverage is enforced via runtime range checks in `Chalk(Options{...})`

## Security and fail-closed tests

- Invalid `level` value (negative or `> 3`) throws `polycpp::Error`.
- Invalid hex string for `hex()` / `bgHex()` collapses to `(0, 0, 0)` (matches upstream lenient behavior); confirms the C++ port does not throw on malformed hex.
- Empty string input returns empty.
- Level 0 returns plain text regardless of chain depth.
- chalk does not strip ANSI bytes from caller-supplied input; tests document this explicitly so callers know to sanitize attacker-controlled text themselves.

## Protocol/client tests

- not applicable because chalk is a string-styling utility with no client/server, wire protocol, database, or external service surface. The `not applicable` decision in `docs/research.md` `## External SDK and native driver strategy` is the authoritative declaration; `scripts/check-port-validation.py` honors it and skips the protocol-test heuristics for this port.

## Release-blocking behaviors

- Exact ANSI escape sequences for every modifier and color produced at level 3 must match upstream byte-for-byte.
- Nested style close-reopen must rewrite inner close codes (the JS-equivalent of `stringReplaceAll(str, close, close + open)` semantics).
- CRLF encasing must close before each `\n`/`\r\n` and reopen after.
- Level 0 must always return plain (or empty) text.
- `ansi256()` must always produce a 256-color sequence regardless of level.
- Constructor must validate the `level` range and throw on misuse.
- `supportsColor` must respect `FORCE_COLOR` and `NO_COLOR` precedence.
- On Windows, `detectColorSupport` must read `polycpp::os::release()`, parse `<major>.<minor>.<build>`, and return level 3 when build >= 14931, level 2 when build >= 10586, otherwise level 1. The release-string threshold parser is covered by `SupportsColorTest.WindowsRelease*`; the actual `polycpp::os::release()` call remains behind `#ifdef _WIN32`.

## Current validation

Record exact commands run, service versions, and notable environment variables.

Last run: 2026-05-03 on Windows against polycpp checkout
`E:\dev\lib\polycpp` using CMake 3.31.11, Visual Studio 17 2022, and
MSVC 19.44.35211. All 93 tests passed.

```powershell
cmake -S . -B build-msvc `
    -DFETCHCONTENT_SOURCE_DIR_POLYCPP=E:\dev\lib\polycpp `
    -DPOLYCPP_SSL_BACKEND=boringssl `
    -DPOLYCPP_UNICODE=builtin `
    -DOPENSSL_NO_ASM=1
cmake --build build-msvc --config Debug --target test_chalk --parallel
ctest --test-dir build-msvc -C Debug --output-on-failure
cmake --build build-msvc --config Debug --parallel
```

`OPENSSL_NO_ASM=1` is required for the Visual Studio generator when using
BoringSSL on this machine; without it, BoringSSL's generated project references
NASM objects that the generator does not assemble. Tests do not require a TTY.
