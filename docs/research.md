# Research

- package: chalk
- npm url: https://www.npmjs.com/package/chalk
- source url: https://github.com/chalk/chalk.git
- upstream version basis: 5.6.2
- upstream revision analyzed: 4ac4288b0b8f8f14ff5511bb661b7502b58ae6af
- upstream default branch: main
- license: MIT
- license evidence: package.json license field and upstream `license` file
- category: terminal styling utility (ANSI escape sequence formatter)

## Package purpose

`chalk` formats strings with ANSI escape codes for foreground colors,
background colors, and text modifiers. Consumers build a chainable
`chalk.red.bold('text')` expression that returns a styled string sized for
the detected terminal capability. Two collaborator packages, vendored
inside chalk 5: `ansi-styles` provides the open/close code tables plus
RGB/hex/256-color converters, and `supports-color` decides which color
level the current environment supports (none, basic 16, ANSI 256, or
truecolor).

## Runtime assumptions

- browser: chalk 5 explicitly drops browser support for the main entry; only the bundled `vendor/supports-color/browser.js` no-op stub matters in browsers
- node.js: chalk 5 requires Node `^12.17.0 || ^14.13 || >=16.0.0`; uses ESM
- filesystem: none at runtime
- network: none
- crypto: none
- terminal: heavy reliance on `process.env` (`TERM`, `COLORTERM`, `FORCE_COLOR`, `NO_COLOR`, CI markers, `TERM_PROGRAM`, `TEAMCITY_VERSION`, `WT_SESSION`, etc.) and `tty.isatty` for level auto-detection

## Dependency summary

- package.json present: yes
- package main: `./source/index.js`
- package types: `./source/index.d.ts`
- package exports: `./source/index.js`
- hard dependencies: none detected in package.json (chalk 5 vendors `ansi-styles` and `supports-color` under `source/vendor/` and references them through the package `imports` map)
- peer dependencies: none detected in package.json
- optional dependencies: none detected in package.json
- dependency analysis report: `docs/dependency-analysis.md`

## Upstream repo layout summary

Clone path used for analysis: `.tmp/upstream/chalk`

Top files:

- `package.json`
- `source/index.js`
- `source/index.d.ts`
- `source/index.test-d.ts`
- `source/utilities.js`
- `source/vendor/ansi-styles/index.js`
- `source/vendor/ansi-styles/index.d.ts`
- `source/vendor/supports-color/index.js`
- `source/vendor/supports-color/index.d.ts`
- `source/vendor/supports-color/browser.js`
- `test/_fixture.js`
- `test/chalk.js`
- `test/instance.js`
- `test/level.js`
- `test/no-color-support.js`
- `test/visible.js`
- `examples/rainbow.js`
- `examples/screenshot.js`
- `benchmark.js`
- `license`

Likely important implementation files:

- `source/index.js`: factory builder, Symbol-keyed proxy chain, `applyStyle`, level guard, multi-arg auto-stringify, RGB/hex/ansi256 dispatch, nested close-reopen logic, line-break encasing
- `source/utilities.js`: `stringReplaceAll` and `stringEncaseCRLFWithFirstIndex`
- `source/vendor/ansi-styles/index.js`: open/close code maps for modifiers, foreground, and background; `wrapAnsi16/256/16m`; `rgbToAnsi256`, `ansi256ToAnsi`, `hexToRgb`, `rgbToAnsi`, `hexToAnsi256`; `modifierNames`, `foregroundColorNames`, `backgroundColorNames`, `colorNames`
- `source/vendor/supports-color/index.js`: env-variable heuristics for color support (FORCE_COLOR, NO_COLOR, CI markers, TERM, COLORTERM, TERM_PROGRAM, TEAMCITY_VERSION, Windows release detection)
- `source/index.d.ts`: TypeScript declaration of the public surface, including the literal-union `ColorSupportLevel`, `ChalkInstance`, `ColorName`, `ModifierName`, and the global `chalk`/`chalkStderr`

## Entry points used by consumers

- `./source/index.js` (default and named exports: `chalk`, `Chalk`, `chalkStderr`, `supportsColor`, `supportsColorStderr`, modifier and color name arrays)
- TypeScript declarations inspected: `./source/index.d.ts`, `./source/vendor/ansi-styles/index.d.ts`, `./source/vendor/supports-color/index.d.ts`

Tests, fixtures, examples, and docs directories:

- `test`: ava-driven compatibility tests organised by behavior
- `examples`: small demo scripts (rainbow, screenshot)

## Important files and why they matter

- `source/index.js`: source of truth for chain semantics (Symbol proxy chain, close-reopen for nested codes, CRLF encasing, multi-arg join, level gating, `visible`)
- `source/vendor/ansi-styles/index.js`: source of truth for ANSI code numbers, color conversion math, and exported name arrays
- `source/vendor/supports-color/index.js`: defines the env-variable detection rules; this file dictates the entire detection pipeline of the C++ port
- `source/index.d.ts`: declares the public TypeScript surface, including modifier/color literal unions and the `Options` shape
- `test/chalk.js`, `test/level.js`, `test/instance.js`, `test/no-color-support.js`, `test/visible.js`: codify exact ANSI output for nesting, multi-arg, level switching, instance independence, and `visible` semantics
- `license`: MIT notice required for `THIRD_PARTY_LICENSES.md`

## Files likely irrelevant to the C++ port

- `benchmark.js`: matcha-based perf harness, not part of behavior contract
- `examples/screenshot.js`: graphical screenshot demo
- `source/index.test-d.ts`: TypeScript-only type assertions; informative for public surface but not runtime behavior
- `media/*`: marketing assets
- `.github/`, `.npmrc`, `.gitattributes`, `.editorconfig`: upstream tooling
- `source/vendor/supports-color/browser.js`: chalk 5 browser entry returns false; the C++ port has no browser target

## Test directories worth mining first

- `test/chalk.js`: nesting, multi-style chains, multi-arg auto-stringify, line breaks, reset-wrapping, falsy values
- `test/visible.js`: `visible` level gating across `visible.red` and `red.visible`
- `test/level.js`: level mutation propagation, color downsampling at level 1/2, ansi256 always-256 behavior
- `test/instance.js`: independent `Chalk` instances do not share level state
- `test/no-color-support.js`: behavior when `supportsColor` reports level 0

## Implementation risks discovered from the source layout

- Nested-style behavior depends on rewriting the inner styler's close codes to `close + open` (the upstream `stringReplaceAll` keeps the search and appends). A naive `replace` (eat and replace) would silently break ANSI nesting; the C++ port must port the upstream semantics exactly.
- CRLF encasing closes the style before each `\n` and reopens it after; missing the `\r` companion or applying it to single-line strings will diverge from upstream output.
- ANSI256 method must always emit a 256-color sequence regardless of detected level (upstream falls through `getModelAnsi` to `wrapAnsi256`). Downsampling here would diverge from `test/level.js`.
- The level option must validate the integer range `[0, 3]` and throw a `TypeError`-like error; coercing or clamping silently would diverge from upstream behavior.
- `supports-color` env-variable heuristics include CI-vendor matching, TEAMCITY semver minor parsing, TERM glob/case-insensitive matching, and Apple/iTerm version parsing. A hand-rolled regex port can easily lose case-insensitivity (the `/i` flag must apply to substring matches as well as anchored prefixes) — this was the cause of an earlier review fix in commit `8a87068`.
- chalk 5 is ESM-only; the published `source/index.js` uses Symbols and `Object.defineProperty` to memoize chain links. The C++ port must achieve the same observable chain semantics without copying the Symbol-keyed implementation strategy.
- Multi-argument calls join with a single space and auto-stringify arrays/numbers/objects via JS coercion. The C++ port must define an explicit conversion policy for the same shapes.

## Companion repo alignment

- companion repos inspected: `cookie`, `ini`, `mime`, `picomatch` (small utility ports); `commander`, `iconv-lite` (API-rich ports); `content-type` (string-only port with planning docs)
- CMake target and alias pattern: `polycpp_chalk` library with `polycpp::chalk` alias; matches the `polycpp_<name>` / `polycpp::<name>` convention
- public header layout: top-level `include/polycpp/chalk/chalk.hpp`, `include/polycpp/chalk/ansi_styles.hpp`, `include/polycpp/chalk/supports_color.hpp` declare the public surface; matches the `polycpp/<lib>/<lib>.hpp` style
- detail/private header strategy: implementation lives under `include/polycpp/chalk/detail/` (`chalk.hpp`, `ansi_styles.hpp`, `supports_color.hpp`) so that public headers carry only declarations and Doxygen comments; matches the `picomatch`/`iconv-lite` split between public and detail headers
- aggregator header strategy: `include/polycpp/chalk/detail/aggregator.hpp` includes all public + detail headers and is used by `src/chalk.cpp` only. Public consumers include the granular public headers.
- examples strategy: `examples/rainbow.cpp`, `examples/diff_table.cpp`, `examples/probe_terminal.cpp` mirror the upstream `examples/rainbow.js` plus two C++-specific demos for color downsampling and capability probing
- documentation site strategy: Doxygen + Breathe + Sphinx scaffold under `docs/sphinx/`; placeholder pages have already been replaced with real getting-started, guides, examples, and API pages
- README structure aligned with companion repos: short purpose, Status, Build, Usage, License sections (mirroring `cookie`, `content-type`)
- deliberate deviations from existing companions: none beyond the public/detail header split that several other companions also use

## Polycpp ecosystem reuse analysis

- polycpp core paths inspected: `include/polycpp/core/error.hpp`, `include/polycpp/core/math.hpp`, `include/polycpp/core/number.hpp`, `include/polycpp/process.hpp`, `include/polycpp/platform/console.hpp`, `include/polycpp/platform/detail/posix/console_traits.hpp`, `include/polycpp/platform/detail/win32/console_traits.hpp`, `include/polycpp/util/util.hpp` (the existing `polycpp::util::format` color-aware helpers).
- polycpp capability snapshot: base polycpp checkout at `/data/work/gitlab-workspace/polycpp` HEAD `7a8df099e2564ff55729a1d2121feb9a88501119` from `git -C /data/work/gitlab-workspace/polycpp rev-parse HEAD` on 2026-05-01. `polycpp::process::getenv`, `polycpp::Math::round/floor/max`, `polycpp::Number::parseInt`/`Number::isNaN`, `polycpp::Error`, and `polycpp::platform::{isTerminal,getTermType,supportsColor}` are all available in that snapshot.
- transport/listener capability review: not applicable. chalk has no socket, listener, TLS, or stream transport surface; the C++ port consumes a raw POSIX/Windows file descriptor only for `isatty`. The current snapshot exposes `polycpp::io::TcpAcceptor`, `polycpp::io::PipeAcceptor`, `polycpp::io::StreamAcceptor`, and `polycpp::tls::*` — none of which are used here. No deferred socket or TLS work exists.
- polycpp core types/functions selected: `polycpp::process::getenv` (env access), `polycpp::Number::parseInt`/`Number::isNaN` (FORCE_COLOR / TERM_PROGRAM_VERSION / Windows release parsing), `polycpp::Math::round`/`Math::floor`/`Math::max` (color downsampling), `polycpp::Error` (level validation throw), `polycpp::platform::isTerminal` (cross-platform `isatty` wrapper), `polycpp::os::release` (Windows-only build-number detection used by `detectColorSupport`).
- polycpp core types/functions rejected: `polycpp::platform::supportsColor(fd)` returns only a boolean and would throw away the 0/1/2/3 level; chalk requires the level so it owns its own env-variable ladder. `polycpp::Buffer`, `polycpp::Stream`, `polycpp::http::Headers`, `polycpp::URL`, `polycpp::JSON`, and `polycpp::events` are not needed because chalk has no buffer/stream/HTTP/URL/JSON/event surface.
- public polycpp interop review: ANSI string output is byte-oriented `std::string`; no JSON or Date interop applies; `Options` is a one-field struct so no `toJSON`/`toObject` adapter is justified.
- string policy: use `std::string` (UTF-8). The ANSI escape pipeline is byte-oriented and chalk does not depend on JavaScript UTF-16 code-unit semantics, so `polycpp::String` is intentionally not used.
- JsonValue/Object/Array policy: not applicable; chalk has no JSON-shaped public surface
- Date/time interop policy: not applicable
- diagnostic/config object policy: `Options` is `struct { std::optional<int> level; }`; a `toJSON()` adapter is unnecessary because the option set is one field
- toJSON/stringify policy: not applicable; `Chalk` instances do not have a JSON projection
- companion libs inspected for reusable APIs: small string-only companions (`cookie`, `content-type`); no existing companion owns terminal capability detection
- companion libs selected for reuse: none. There is no existing polycpp companion that owns ANSI escape code rendering or terminal capability detection.
- companion libs rejected or deferred: none
- new local abstractions introduced: `polycpp::chalk::Chalk` (chainable styler), `polycpp::chalk::Options`, `polycpp::chalk::Styler` (linked-list chain node), `polycpp::chalk::ansi::AnsiCode`, `polycpp::chalk::ansi::wrapAnsi16/256/16m`, `polycpp::chalk::ansi::rgbToAnsi256/hexToRgb/ansi256ToAnsi/hexToAnsi256/rgbToAnsi/makeColorCode/makeColorCode256`, `polycpp::chalk::ColorSupport`, `polycpp::chalk::detectColorSupport`, `polycpp::chalk::supportsColor`, `polycpp::chalk::supportsColorStderr`, `polycpp::chalk::chalk()`, `polycpp::chalk::chalkStderr()`, modifier/foreground/background/color name arrays. These are package-specific behavior that no other polycpp module owns.
- reuse risks or integration gaps: none material. The companion uses `polycpp::platform::isTerminal(fd)` for cross-platform `isatty` and owns the env-variable ladder for color-level detection because `polycpp::platform::supportsColor(fd)` only returns a boolean.

## Node parity surface audit

- callback APIs: not applicable; chalk has no async callback surface. The analyzer flagged 2 `callback-argument` signals which correspond to JavaScript array/iterator callbacks (`for (const [styleName, style] of Object.entries(...))` style code), not public API callbacks. Decision: omit; recorded in `docs/divergences.md` under non-meaningful runtime constructs.
- Promise APIs: not applicable; no async API
- EventEmitter APIs: not applicable
- server/listener APIs: not applicable; chalk has no listen modes, no TCP/Unix path/TLS sockets, and no adopted-handle path. No transport or listener primitives to wire up.
- stream APIs: not applicable; chalk operates on whole strings. No object-mode chunking, no NDJSON adapter, no transform stream needed.
- Buffer and binary APIs: not applicable; output is `std::string`
- URL, timer, process, and filesystem APIs: `process.env` maps to `polycpp::process::getenv`; `tty.isatty(fd)` maps to `polycpp::platform::isTerminal(fd)`; `os.release()` (used by upstream for the Windows 10 build 10586/14931 truecolor branch) maps to `polycpp::os::release()` and the same parsing logic. There are no connect, per-command, pool, or idle deadlines.
- crypto, compression, TLS, network, and HTTP APIs: not applicable. No client transport, no server/listener lifecycle, no Unix/IPC path support, and no TLS client/server modes — chalk does not touch any of these surfaces.
- unsupported Node-specific APIs and audit reason: the JavaScript Symbol-based proxy chain (`Symbol('GENERATOR')`, `Symbol('STYLER')`, `Symbol('IS_EMPTY')`) and the `Object.setPrototypeOf` trick that makes a Chalk instance both callable and a function are not meaningful in C++; we model them as `Chalk::operator()` plus a private `Styler` linked list. The `sniffFlags` option on `createSupportsColor` reads `process.argv` for `--color`/`--no-color`/`--color=256`/`--color=truecolor` — a Node convention where any library can parse the host's command line. In C++ this is a layering violation, so the option is intentionally not provided; callers parse their own CLI and translate the result into `Options{.level = N}`. `FORCE_COLOR` and `NO_COLOR` env vars remain the supported override path.

## External SDK and native driver strategy

- upstream external services/protocols: none. chalk emits ANSI escape sequences to a string; it does not drive an external SDK or wire protocol.
- native SDKs/client libraries to use: none for the styling surface. Only the platform `isatty` call is used directly.
- SDKs/protocols explicitly not reimplemented: not applicable; there is nothing to reimplement
- adapter/linking strategy: link `polycpp` only. The platform `isatty` is provided by `<unistd.h>` (POSIX) or `<io.h>` (Windows); no extra `find_package` or third-party dependency is added.
- test environment needs: none beyond CMake + GoogleTest. Tests intentionally do not assume a TTY, so `supportsColor` tests probe both `fd 1` and `fd 2` without asserting a specific level.

## Compatibility foundation review

- downstream dependency role: foundational utility for downstream tooling that wants colorful CLI output (e.g. CLI ports such as `commander`, log formatters)
- native substitution risk: medium. A naive ANSI library could differ from chalk on nested close-reopen behavior, CRLF encasing, multi-arg join, and color downsampling. We treat chalk as a behavior-compatibility port, not a `printf("\x1b[...m")` replacement.
- upstream implementation data to preserve: ANSI open/close code tables, modifier/foreground/background name arrays, RGB-to-ANSI256 and ANSI256-to-ANSI conversion math, hex-to-RGB shorthand expansion, level-aware `makeColorCode` dispatch, and `ansi256()` always emitting a 256-color sequence
- generated or vendored data plan: not applicable. The upstream code tables are small constexpr literals (`AnsiCode RED = {31, 39}` etc.) hand-translated into `include/polycpp/chalk/ansi_styles.hpp`; there is no large generated artifact, so no separate translation unit is required. If upstream renames a code, regeneration is a manual `git diff source/vendor/ansi-styles/index.js` review.
- compatibility fixture strategy: copy upstream `test/*.js` ANSI byte assertions verbatim into GoogleTest cases, asserting exact escape sequences (`\x1B[31mfoo\x1B[39m` etc.). Tests live in `tests/test_chalk.cpp`.

## Security and fail-closed review

- security-sensitive behavior: none. chalk decorates strings with ANSI escape codes for terminals; it does not authenticate users, sign or encrypt data, parse untrusted protocol input, or evaluate code. Analyzer `securityHints.securitySensitive` is `false`.
- trust boundary: caller-owned text and caller-owned color values; environment variables and TTY status are read from the host process.
- supported protocol or algorithm matrix: not applicable
- unsupported behavior and fail-closed policy: invalid `level` values throw `polycpp::Error`. Invalid hex strings collapse to `(0, 0, 0)` (matching upstream behavior of treating malformed hex as black). Color values outside `0..255` are passed through to the ANSI sequence — this matches upstream behavior, which neither clamps nor throws.
- result-set/framing drain policy, if protocol client: not applicable
- binary payload type-mapping policy, if protocol client: not applicable
- stateful parser/session-state policy, if protocol client/server: not applicable
- server/listener response writer matrix, if protocol server surface exists: not applicable
- key, secret, credential, or user-controlled input handling: chalk does not handle secrets; there is no credential surface. ANSI escape characters injected into user-controlled text are not stripped (matching upstream behavior); callers that style attacker-controlled strings should sanitize the input themselves.
- misuse cases that must be tested: invalid level (negative, above 3) must throw; empty string returns empty; level 0 returns plain text; nested styles must not bleed.

## Core use cases

- Quickly color or modify terminal output: `chalk.red("err")`, `chalk.bold("hi")`.
- Build a reusable theme: `auto error = chalk.bold().red(); std::cout << error("nope");`.
- Use truecolor or hex with auto-downsampling on lower-capability terminals: `chalk.hex("#FFA500")`.
- Detect terminal color support to decide whether to color: `polycpp::chalk::supportsColor()`.
- Use the global `chalk()` instance with auto-detected level for stdout, `chalkStderr()` for stderr.

## Key features to port first

- Chalk constructor + `Options{level}` validation
- Modifier methods (reset, bold, dim, italic, underline, overline, inverse, hidden, strikethrough)
- Foreground/background standard 16 + bright 16 colors plus `gray`/`grey` aliases
- `rgb()`, `hex()`, `ansi256()` and their `bg*` counterparts with level-aware downsampling
- `visible()` modifier (level gate)
- Multi-argument call operator with auto-stringification and space join
- Nested style close-reopen and CRLF encasing
- `supportsColor`, `supportsColorStderr`, `detectColorSupport(fd)`
- Global `chalk()` and `chalkStderr()` instances
- Modifier/foreground/background/color name arrays

## Features to defer

- Browser-environment supports-color stub (`source/vendor/supports-color/browser.js`); not applicable in C++

## v0 scope

- port version: 1.0.0
- versioning note: port version is independent from upstream versioning
- supported APIs: `Chalk`, `Options`, `chalk()`, `chalkStderr()`, all modifier and color chain methods, `rgb`/`hex`/`ansi256`/`bgRgb`/`bgHex`/`bgAnsi256`, `visible`, `level()`/`setLevel()`, multi-arg `operator()`, `ColorSupport`, `detectColorSupport`, `supportsColor`, `supportsColorStderr`, `modifierNames`, `foregroundColorNames`, `backgroundColorNames`, `colorNames`, `ansi::*` low-level helpers
- unsupported APIs: JavaScript-specific Symbol-based chain extension, `Object.defineProperty` proxy memoization, browser entry for supports-color, old Windows 10 release detection
- dependency plan: no new companion libraries are required; the port depends only on base `polycpp` (`process`, `core/error`, `core/math`, `core/number`)
- polycpp modules to use: `polycpp::process`, `polycpp::Math`, `polycpp::Number`, `polycpp::Error`
- missing polycpp primitives: none. `polycpp::platform::isTerminal(fd)` covers the cross-platform `isatty` need.
