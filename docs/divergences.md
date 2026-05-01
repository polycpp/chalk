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

## Unsupported Runtime-Specific Features

- JavaScript `Symbol('GENERATOR')`, `Symbol('STYLER')`, `Symbol('IS_EMPTY')` brand keys: replaced by private members on the `Chalk` class. Symbols are a JavaScript runtime construct without a C++ equivalent.
- `Object.setPrototypeOf` callable-instance trick that makes a chalk instance both a function and an object: replaced by C++ `operator()`.
- `Object.defineProperty` memoization on first chain access: not necessary in C++. Chain creation is a constant-time `shared_ptr<Styler>` link with no Symbol bookkeeping.
- ESM-only `imports` map (`#ansi-styles`, `#supports-color`): C++ resolves these vendored modules at compile time through `include/polycpp/chalk/ansi_styles.hpp` and `include/polycpp/chalk/supports_color.hpp`. The package-imports indirection is a JavaScript packaging detail.
- Browser `supports-color` stub (`source/vendor/supports-color/browser.js`): C++ has no browser target.
- The single analyzer-flagged Node parity signal (`callback-argument`) is not surfaced as a public callback API; the matched calls are JS-internal `Object.entries(...).forEach(...)`-style iteration, not part of chalk's contract.
