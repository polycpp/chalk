# Test Plan

## Unit tests

- ANSI escape generation: `wrapAnsi16`, `wrapAnsi256` (foreground and background), `wrapAnsi16m` (foreground and background) emit the expected escape strings.
- RGB/256/hex math: `rgbToAnsi256` for both grayscale and color regions; `ansi256ToAnsi` for `0..7`, `8..15`, `16..231`, `232..255`; `hexToRgb` for 6-char, 3-char, lowercase, and invalid input; `rgbToAnsi` end-to-end.
- `makeColorCode` truecolor (level 3), 256 (level 2), 16 (level 1) for both foreground and background; `makeColorCode(level, hex)` overload; `makeColorCode256` always emits a 256-color sequence regardless of level.
- `Chalk` construction defaults to level 0; `Options{level=N}` sets level N; out-of-range levels throw `polycpp::Error`; `setLevel` updates the level on an existing instance.
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

- not applicable because chalk is a string-styling utility with no client/server, wire protocol, database, or external service surface.
- Real service-backed e2e coverage: not applicable; this is not an external service, database, or network protocol client.
- Stateful parser/session-state coverage: not applicable; chalk has no protocol parser or session-state surface.
- Response writer coverage: not applicable; chalk has no protocol server surface.
- Binary row / prepared/binary coverage: not applicable; chalk has no binary protocol surface.
- Auth / TLS / transport / malformed-packet / fail-closed coverage: not applicable; chalk has no auth, TLS, transport, or protocol-packet surface. Misuse cases for invalid `level` and invalid hex are covered under `## Security and fail-closed tests`.

## Release-blocking behaviors

- Exact ANSI escape sequences for every modifier and color produced at level 3 must match upstream byte-for-byte.
- Nested style close-reopen must rewrite inner close codes (the JS-equivalent of `stringReplaceAll(str, close, close + open)` semantics).
- CRLF encasing must close before each `\n`/`\r\n` and reopen after.
- Level 0 must always return plain (or empty) text.
- `ansi256()` must always produce a 256-color sequence regardless of level.
- Constructor must validate the `level` range and throw on misuse.
- `supportsColor` must respect `FORCE_COLOR` and `NO_COLOR` precedence.
- On Windows, `detectColorSupport` must read `polycpp::os::release()`, parse `<major>.<minor>.<build>`, and return level 3 when build >= 14931, level 2 when build >= 10586, otherwise level 1. The branch is `#ifdef _WIN32` and is exercised manually on Windows; Linux CI does not exercise it.

## Current validation

Record exact commands run, service versions, and notable environment variables.

Last run: 2026-05-02 against polycpp checkout `/data/repo/polycpp-old`
(HEAD `93a4f2f3`) using its `_deps` cache for asio/boringssl/brotli/
fast_float/googletest/nghttp2/quickjs_engine/zlib/zstd. All 89 tests
passed (81 pre-existing + 8 new regression tests added for AF-B / AF-C /
AF-D in commit landing on master 2026-05-02).

```bash
DEPS=/data/repo/polycpp-old/build-clang14-gap-debug/_deps
cmake -S /data/work/lib/chalk -B /data/work/lib/chalk/build -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_COMPILER=/usr/bin/clang++-14 \
    -DCMAKE_C_COMPILER=/usr/bin/clang-14 \
    -DFETCHCONTENT_SOURCE_DIR_POLYCPP=/data/repo/polycpp-old \
    -DFETCHCONTENT_SOURCE_DIR_ASIO="$DEPS/asio-src" \
    -DFETCHCONTENT_SOURCE_DIR_BORINGSSL="$DEPS/boringssl-src" \
    -DFETCHCONTENT_SOURCE_DIR_BROTLI="$DEPS/brotli-src" \
    -DFETCHCONTENT_SOURCE_DIR_GOOGLETEST="$DEPS/googletest-src" \
    -DFETCHCONTENT_SOURCE_DIR_ZLIB="$DEPS/zlib-src" \
    -DFETCHCONTENT_SOURCE_DIR_ZSTD="$DEPS/zstd-src" \
    -DPOLYCPP_IO=asio \
    -DPOLYCPP_SSL_BACKEND=boringssl \
    -DPOLYCPP_UNICODE=icu
cmake --build /data/work/lib/chalk/build -j"$(nproc)" -t test_chalk
( cd /data/work/lib/chalk/build && ctest --output-on-failure )
python3 /data/work/lib/chalk/docs/build.py
```

The `_build_and_test.sh` helper at `/data/work/lib/_build_and_test.sh`
encodes the same invocation but defaults to `/data/repo/polycpp` for the
polycpp source path; pass `-DFETCHCONTENT_SOURCE_DIR_POLYCPP=...` if the
default checkout is missing the `_deps` cache.

Environment: clang++ 14 with C++20 standard; no service dependencies;
tests do not require a TTY.
