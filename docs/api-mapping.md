# API Mapping

| Upstream symbol | C++ symbol | Status | Notes |
|---|---|---|---|
| `chalk` (default export) | `polycpp::chalk::chalk()` (returns `Chalk&`) | direct | Singleton with auto-detected level for fd 1; mirrors the upstream module-level default export |
| `chalkStderr` | `polycpp::chalk::chalkStderr()` (returns `Chalk&`) | direct | Singleton for fd 2 with auto-detected level |
| `Chalk` (constructor) | `polycpp::chalk::Chalk` (class) | direct | C++ class; default and `Options{level}` constructors. Throws `polycpp::Error` when level is outside `[0, 3]`. |
| `Options.level` | `polycpp::chalk::Options::level` (`std::optional<int>`) | direct | `std::optional` because the JS field is optional and triggers auto-detection when absent |
| `chalkInstance(...text)` (callable) | `Chalk::operator()(string)` and variadic `operator()(T, Args...)` | adapted | JavaScript joins with `" "` and string-coerces every argument; the C++ overload joins with `" "` and string-converts via `std::to_string` for arithmetic types and direct `std::string` for char-like types. Object types must be convertible to `std::string`. |
| chain getter `chalk.red`, `chalk.bgGreen`, etc. (Symbol-keyed proxy) | `Chalk::red()`, `Chalk::bgGreen()`, etc. (member functions) | adapted | C++ has no JavaScript-style getter chain; explicit member functions return a new `Chalk` with an additional styler layer. Each call reuses the previous chain via a `std::shared_ptr<const Styler>` parent pointer. |
| `chalk.gray` / `chalk.grey` (alias for `blackBright`) | `Chalk::gray()` / `Chalk::grey()` | direct | Both map to `blackBright()`; recorded in `chalk.hpp` aliases |
| `chalk.bgGray` / `chalk.bgGrey` | `Chalk::bgGray()` / `Chalk::bgGrey()` | direct | Both map to `bgBlackBright()` |
| `chalk.visible` | `Chalk::visible()` | direct | Returns a `Chalk` with `isEmpty=true`; `applyStyle` returns "" when level==0 even if styles were applied |
| `chalk.rgb(r, g, b)` | `Chalk::rgb(int, int, int)` | direct | Foreground RGB; level-aware downsampling through `ansi::makeColorCode` |
| `chalk.hex(color)` | `Chalk::hex(const std::string&)` | direct | Accepts `#RRGGBB`, `RRGGBB`, `#RGB`, `RGB`; invalid input collapses to `(0, 0, 0)` to match upstream lenient behavior |
| `chalk.ansi256(index)` | `Chalk::ansi256(int)` | direct | Always emits `\x1B[38;5;<n>m` regardless of detected level (matches upstream `getModelAnsi` fall-through) |
| `chalk.bgRgb`, `chalk.bgHex`, `chalk.bgAnsi256` | `Chalk::bgRgb`, `Chalk::bgHex`, `Chalk::bgAnsi256` | direct | Same dispatch as foreground variants but with `\x1B[49m` close |
| `chalk.level` (read/write) | `Chalk::level()` getter and `Chalk::setLevel(int)` setter | adapted | C++ uses explicit accessors instead of a writable property to avoid a public mutable data member |
| `chalk.level` propagation across chain (JS proxy) | `Chalk` chains share the level captured at construction; mutating the parent's level does not retroactively change derived chains | adapted | Documented divergence; recorded in `docs/divergences.md` |
| `ansiStyles.modifierNames`, `foregroundColorNames`, `backgroundColorNames`, `colorNames` | `polycpp::chalk::modifierNames()`, `foregroundColorNames()`, `backgroundColorNames()`, `colorNames()` | direct | Free functions returning `const std::vector<std::string>&`; same names and order as upstream |
| `ansiStyles.color.*` / `ansiStyles.bgColor.*` open/close codes | `polycpp::chalk::ansi::AnsiCode` constants in `ansi_styles.hpp` (`RED`, `BG_RED`, ...) | direct | Constexpr literal pairs of open/close numbers; same set and same numeric values |
| `ansiStyles.codes` map | implicit through the `AnsiCode::close` field on each constant | adapted | C++ stores close codes inline on each style constant rather than in a separate `Map` |
| `wrapAnsi16(code)` | `polycpp::chalk::ansi::wrapAnsi16(int)` | direct | Returns `\x1B[<code>m` |
| `wrapAnsi256(code)` / `bgWrapAnsi256(code)` | `polycpp::chalk::ansi::wrapAnsi256(int code, bool background=false)` | direct | Single function with a `background` flag to avoid duplicate symbols |
| `wrapAnsi16m(r,g,b)` / `bgWrapAnsi16m(...)` | `polycpp::chalk::ansi::wrapAnsi16m(int r, int g, int b, bool background=false)` | direct | Same two-arg style as `wrapAnsi256` |
| `rgbToAnsi256(r,g,b)`, `hexToRgb(hex)`, `ansi256ToAnsi(code)`, `hexToAnsi256(hex)`, `rgbToAnsi(r,g,b)` | `polycpp::chalk::ansi::rgbToAnsi256`, `hexToRgb`, `ansi256ToAnsi`, `hexToAnsi256`, `rgbToAnsi` | direct | Same color conversion math; uses `polycpp::Math::round/floor/max` for parity with JS `Math.*` |
| `getModelAnsi(model, level, type, ...args)` | `polycpp::chalk::ansi::makeColorCode` overloads + `makeColorCode256` | adapted | C++ exposes typed overloads instead of a stringly-typed dispatcher |
| `supportsColor` (default export from vendored module) | `polycpp::chalk::supportsColor()` (`const ColorSupport&`) | direct | Cached for fd 1 |
| `supportsColorStderr` | `polycpp::chalk::supportsColorStderr()` (`const ColorSupport&`) | direct | Cached for fd 2 |
| `ColorInfo`, `ColorSupportLevel` (`source/vendor/supports-color/index.d.ts`) | `polycpp::chalk::ColorSupport` struct (`level`, `hasBasic`, `has256`, `has16m`) | direct | Typed C++ struct mirrors the upstream object shape |
| `createSupportsColor(stream, options)` | `polycpp::chalk::detectColorSupport(int fd)` | adapted | Upstream takes a Node stream + options; the C++ port takes a raw fd because there is no Node stream object. The `sniffFlags` argv-parsing option is intentionally omitted because reading the host process's command line is a layering violation in a C++ library; callers parse their own CLI flags and translate the result into `Options{.level = N}`. See `docs/divergences.md` `## Unsupported Runtime-Specific Features`. |
| `Symbol('GENERATOR')`, `Symbol('STYLER')`, `Symbol('IS_EMPTY')` | private members `styler_`, `level_`, `isEmpty_` on `Chalk` | omitted | JavaScript Symbol-based brand keys are not meaningful in C++ |
| `Object.setPrototypeOf` to make `chalk` callable | `Chalk::operator()` | omitted | C++ does not need prototype manipulation; explicit operator() is the natural mapping |
| `Object.defineProperty(builder, name, ...)` memoization on first chain access | implicit; chain creation is cheap (`shared_ptr<Styler>` link) | omitted | C++ chain links are constant-time and rebuilt eagerly; no caching is required |
| `stringReplaceAll(string, search, replace)` | `polycpp::chalk::detail::replaceCloseWithReopen` | direct | Same semantics as upstream: keep `search`, append `replace` after every occurrence |
| `stringEncaseCRLFWithFirstIndex(string, prefix, suffix, lfIndex)` | `polycpp::chalk::detail::encaseCRLF` | direct | Same semantics as upstream; closes before each `\n`, reopens after, keeps `\r` companion |
| browser `supports-color` (`source/vendor/supports-color/browser.js`) | not provided | omitted | C++ has no browser target |
| `os.release()` Windows 10 truecolor branch | `polycpp::os::release()` parsed and dispatched inside `detectColorSupport` (Windows-only `#ifdef _WIN32`) | direct | Mirrors upstream: build >= 10586 → 256-color; build >= 14931 → truecolor; otherwise level 1 |
| upstream test-only TypeScript file `source/index.test-d.ts` | n/a | omitted | TypeScript-only assertions; C++ has its own GoogleTest suite |

Status values:

- `direct`: same behavior with an idiomatic C++ spelling.
- `compatibility layer`: same user-facing behavior through a different
  implementation shape.
- `adapted`: preserves the upstream intent with a typed C++ API.
- `deferred`: planned future work that is intentionally not implemented yet.
- `omitted`: deliberately not part of this port.

## TypeScript Declaration Review

- Declaration source used: `source/index.d.ts` plus the two vendored declaration files in `source/vendor/ansi-styles/index.d.ts` and `source/vendor/supports-color/index.d.ts`
- Public APIs, overloads, options, callbacks, streams, or literal unions found only or most clearly in declarations: the `ColorSupportLevel` literal union (`0 | 1 | 2 | 3`) is declared only in `*.d.ts`; the C++ port enforces `[0, 3]` via runtime validation. The `ChalkInstance` callable signature `(...text: unknown[]) => string` is declared only in TypeScript; the C++ port models it as a variadic `operator()`. The `ModifierName`, `ForegroundColorName`, `BackgroundColorName`, and `ColorName` literal unions surface as the four name array accessors.
- Declaration-only globals, caches, deprecated fields, or runtime-specific surfaces mapped as unsupported/not-applicable: the JavaScript callable-instance `level` writable field corresponds to `Chalk::setLevel`; we do not expose a writable public data member. The dynamic chain getter style `chalk.red.bold` exists only at the TypeScript level via `ChalkInstance`; the C++ port uses explicit member functions and records this as `adapted`.

## Framework object boundary review

- Upstream reads or mutates framework/request/response/context objects: no
- Upstream fields or methods read: none
- Upstream fields or methods written: none
- C++ adapter boundary: not applicable. chalk has no framework-object boundary; the public API operates on `std::string` text plus typed `Options` and `ColorSupport` structs.
- Partial mutation risk on validation failure: not applicable; the C++ API does not mutate caller-owned objects. Constructor validation throws before any state is observable.

The boundary does not overlap with any existing `polycpp` request/response/header/stream/URL surface; chalk does not need one.

## Node parity surface review

- Callback APIs: not used. Analyzer flagged `callback-argument` signals from JS array-iteration callbacks inside chalk's factory; these are JavaScript metaprogramming, not public callback APIs. No C++ public callback surface.
- Promise APIs: not used; chalk is synchronous
- EventEmitter APIs: not used
- Server/listener APIs: not used. chalk has no listen modes (TCP, Unix/IPC path, adopted handles, TLS) and no accepted connection types; no C++ adapter required.
- Diagnostic/tracing APIs: not used
- Stream APIs: not used. chalk operates on whole strings; no object-mode chunking, no NDJSON adapter, no Transform stream.
- Buffer and binary APIs: not used; the public surface returns `std::string`
- URL, timer, process, and filesystem APIs: only `process.env` and `tty.isatty(fd)` are read inside `supports-color`. `process.env` maps to `polycpp::process::getenv`; `isatty` uses platform headers (`<unistd.h>` / `<io.h>`). No connect, per-command, pool, or idle deadlines; chalk is synchronous and does not own any resource that needs a deadline.
- Crypto, compression, TLS, network, and HTTP APIs: not used. No client transport, no server/listener lifecycle, no Unix/IPC path support, no TLS client/server modes.
- Unsupported or non-meaningful Node-specific APIs and audit reason: JavaScript Symbol-keyed proxy chain (`Symbol('GENERATOR')`, `Symbol('STYLER')`, `Symbol('IS_EMPTY')`) and `Object.setPrototypeOf` callable trick — both are JavaScript runtime affordances rather than meaningful C++ library APIs. The C++ port models them as `Chalk::operator()` plus a `Styler` linked list. `os.release()`-based old Windows 10 truecolor detection is deferred (modern detection still works for current Windows Terminals).
