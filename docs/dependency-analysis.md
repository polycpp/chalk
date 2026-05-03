# Dependency and JavaScript API Analysis

- package: chalk
- package version: 5.6.2
- package root: `.tmp/upstream/chalk`
- analyzer json: `.tmp/dependency-analysis.json`
- published npm artifact path: `.tmp/npm-package`
- published npm artifact analyzed: yes; resolved version 5.6.2 matches the upstream `v5.6.2` git tag
- include dev dependencies: no
- dependency source install used: `npm install --ignore-scripts` (analyzer-managed); no scripts run from upstream
- companion root checked: `<companion libs root>`

## Package entry metadata

- main: `./source/index.js`
- module: not declared (the package is ESM-only via `"type": "module"`)
- types: `./source/index.d.ts`
- exports: `./source/index.js`
- bin: not declared
- missing declared entries in repo clone: none. Both the git checkout and the published npm tarball expose `source/index.js`, `source/index.d.ts`, `source/utilities.js`, and the `source/vendor/{ansi-styles,supports-color}/` directories used through the package `imports` map.
- TypeScript source files detected: 1 (`source/index.test-d.ts`)
- TypeScript declarations reviewed: `source/index.d.ts` (chalk public API), `source/vendor/ansi-styles/index.d.ts`, `source/vendor/supports-color/index.d.ts`
- declaration-source decision: rely on the upstream `*.d.ts` files (the package ships them); they are the clearest contract for `Options`, `ChalkInstance`, `ColorSupportLevel`, and the modifier/color literal unions
- source-vs-published artifact decision: published tarball matches the git source tree byte-for-byte for the inspected runtime files; analysis is performed against the git clone

## Direct dependencies

- none. chalk 5 vendors `ansi-styles` and `supports-color` under `source/vendor/` and references them through the package `imports` map (`#ansi-styles`, `#supports-color`). They are not external runtime dependencies. `package.json` lists no `dependencies`, `peerDependencies`, or `optionalDependencies`.

## Dependency ownership decisions

For every direct runtime dependency, choose exactly one outcome:

- use existing polycpp companion
- create separate private polycpp companion repo
- implement private helper in this repo
- optional adapter
- deferred or unsupported feature

For every dependency, also choose a license strategy.

| Package | Kind | Requested | Installed | License | License evidence | License impact | License strategy | Affects repo license | Deps | Source files | Node API calls | JS API calls | Recommendation | Rationale |
|---|---|---|---|---|---|---|---|---|---:|---:|---:|---:|---|---|
| ansi-styles (vendored) | runtime (vendored) | `#ansi-styles` -> `./source/vendor/ansi-styles/index.js` | n/a (vendored) | MIT | upstream chalk `package.json` license; vendored source tree retains MIT header in chalk repo `license` and matches `chalk/ansi-styles` MIT | permissive | permissive dependency ok with notice | no | 0 | 1 | 0 | 0 | implement private helper in this repo | chalk 5 vendors ansi-styles inside its tarball; the C++ port re-implements the small constexpr code tables and helpers in `include/polycpp/chalk/ansi_styles.hpp` plus `detail/ansi_styles.hpp`. No separate companion repo is justified because there are no other consumers in the polycpp ecosystem yet. |
| supports-color (vendored) | runtime (vendored) | `#supports-color` -> `./source/vendor/supports-color/index.js` (Node) | n/a (vendored) | MIT | upstream chalk repository ships vendored copy under MIT; original `chalk/supports-color` is also MIT | permissive | permissive dependency ok with notice | no | 1 (`process.env`) | 1 | 0 | 17 | implement private helper in this repo | chalk 5 vendors supports-color inside its tarball; the C++ port re-implements the env-variable detection ladder in `include/polycpp/chalk/supports_color.hpp` plus `detail/supports_color.hpp`. Browser stub is intentionally not ported. |

## License impact summary

- upstream package license: MIT
- repo license decision: MIT, matching base polycpp companion convention; `LICENSE` carries the full MIT text with `polycpp contributors` as the copyright holder
- GPL/AGPL dependencies: none
- LGPL/MPL dependencies: none
- permissive dependencies requiring notices: chalk (MIT). Vendored `ansi-styles` and `supports-color` are also MIT and travel inside the chalk tarball.
- dev/test-only dependencies excluded from shipped artifacts: not applicable; the analyzer was run with `includeDev=false`. Upstream dev deps such as `ava`, `xo`, `c8`, `tsd`, `execa`, `matcha` are not shipped, not linked, and not represented in the C++ port.
- dependency license notices to add to `THIRD_PARTY_LICENSES.md`: chalk MIT notice (already present, sourced from `chalk/chalk` upstream `license` file)

## Transitive dependency summary

- not applicable. chalk 5 has no production npm dependencies; the dependency graph terminates at the chalk package itself.

## Runtime API usage

### Target package

- entry points analyzed: `source/index.js` plus the imported `source/utilities.js` and the two `source/vendor/*` packages reachable through the package `imports` map
- source files analyzed by analyzer: 3 (chalk's reachable JS files; vendored packages are analyzed in-place because the analyzer follows the resolved import paths)
- source files manually inspected: `source/index.js`, `source/utilities.js`, `source/index.d.ts`, `source/vendor/ansi-styles/index.js`, `source/vendor/ansi-styles/index.d.ts`, `source/vendor/supports-color/index.js`, `source/vendor/supports-color/index.d.ts`
- external imports seen from target: `#ansi-styles`, `#supports-color` (both resolved to vendored sources)

### Analyzer porting gates

- polycpp reuse hints consumed: yes. `polycppReuseHints` was an empty list (chalk has no Node module imports the analyzer matched against polycpp modules). Manual review still selected `polycpp::process`, `polycpp::Math`, `polycpp::Number`, and `polycpp::Error` — recorded in `docs/research.md` under `## Polycpp ecosystem reuse analysis`.
- Node parity hints consumed: yes. The single hint (`callbacks` with `signals: ["callback-argument"]`) was inspected; the matched calls are JS array-iteration callbacks inside chalk's factory loop (`Object.entries(...).forEach`-shaped code), not public callback APIs. The C++ port does not need a callback-shaped public API. Decision recorded in `docs/research.md` `## Node parity surface audit` and in `docs/divergences.md`.
- security hints consumed: yes. `securityHints.securitySensitive` is `false`; `matchedTerms`, `cryptoApis`, and `requiredReviews` are empty. The fail-closed review in `docs/research.md` confirms chalk has no security-sensitive surface.
- security-sensitive package: no
- polycpp capability snapshot consumed: yes. polycpp checkout at `/data/work/gitlab-workspace/polycpp` HEAD `7a8df099e2564ff55729a1d2121feb9a88501119` (rev parsed on 2026-05-01) confirms `polycpp::process::getenv`, `polycpp::Math::round/floor/max`, `polycpp::Number::parseInt`/`Number::isNaN`, `polycpp::Error`, and `polycpp::platform::{isTerminal,getTermType,supportsColor}` exist; recorded in `docs/research.md` `## Polycpp ecosystem reuse analysis`. The `polycpp::platform::isTerminal` reuse opportunity is recorded in `docs/divergences.md` audit finding `AF-2026-05-01-A`.
- transport/listener capability hints consumed: yes; not applicable to chalk. There is no socket, listener, or TLS surface to map, so the `transport/listener capability review` field in `docs/research.md` records `not applicable`.

### Node.js API usage

- `process.env` (read in `source/vendor/supports-color/index.js` for FORCE_COLOR, NO_COLOR, TERM, COLORTERM, CI markers, TERM_PROGRAM, TERM_PROGRAM_VERSION, TEAMCITY_VERSION, etc.)
- `tty.hasColors`-style detection through `tty.isatty(fd)` (read in `source/vendor/supports-color/index.js`)
- `os.release()` (read in `source/vendor/supports-color/index.js` for old Windows 10 truecolor detection; mapped to `polycpp::os::release()` in the Windows-only branch and covered by threshold tests)

The analyzer's `nodeApis` was empty for the chalk package because the upstream `index.js` does not directly import Node modules — they are imported only inside the vendored `supports-color`. The analyzer treats vendored files as part of the package, which is why the per-target counts read `0` Node API calls; manual inspection of the vendored file confirms `process.env`, `tty.isatty`, and `os.release` usage above.

### Node parity surface usage

- callbacks: not used in the public surface. The two analyzer signals correspond to JS iteration callbacks inside chalk's factory; not part of the C++ public API.
- Promise APIs: not used
- EventEmitter APIs: not used
- server/listener APIs: not used. No listen modes, accepted connection types, lifecycle events, or C++ adapter required.
- diagnostic/tracing APIs: not used
- streams: not used. chalk operates on whole strings; no object-mode chunk type and no C++ adapter is needed.
- Buffer and binary data: not used; output is `std::string`
- URL/timer/process/filesystem APIs: only `process.env` (mapped to `polycpp::process::getenv`) and `tty.isatty(fd)` (mapped to platform `isatty` from `<unistd.h>` / `<io.h>`). No URL parsing, no timers, no connect deadlines, no per-operation deadlines, no pool/idle deadlines.
- crypto/compression/TLS/network/HTTP APIs: not used. No client transport, no server/listener lifecycle, no Unix/IPC path support, no TLS modes.

### JavaScript API usage

- `Math.round`, `Math.floor`, `Math.max`: used by `ansi-styles` color conversion math (`rgbToAnsi256`, `ansi256ToAnsi`); mapped to `polycpp::Math::round/floor/max`
- `Number.isInteger`, `Number.parseInt`: used by chalk's level validation and `supports-color` env parsing; mapped to `polycpp::Number::parseInt`/`Number::isNaN`. Integer-range checks are expressed directly in C++.
- `Map`, `Map.prototype.set`, `Object.create`, `Object.entries`, `Object.keys`, `Object.defineProperty`, `Object.defineProperties`, `Object.setPrototypeOf`, `Array.prototype.join`, `String.prototype.slice`, `String.prototype.toUpperCase`: JavaScript metaprogramming used to construct the dynamic chalk chain. The C++ port replaces these with a typed `Styler` linked list and explicit chain methods. None require a polycpp primitive.
- `Error`: replaced by `polycpp::Error` for level validation.

### Framework object boundary usage

- analyzer-reported target-package framework object accesses: 0 (`targetFrameworkObjectAccesses` empty)
- analyzer-reported dependency framework object accesses: 0 (`dependencyFrameworkObjectAccesses` empty)
- manual review decision: chalk has no framework-object boundary (no `req`, `res`, `ctx`, `headers`, `socket`). No adapter type is required. The C++ public API exposes `Chalk`, `Options`, `ColorSupport`, and free functions; none read or mutate caller-owned framework objects.

## Porting decisions

- vendored `ansi-styles`: re-implement as `polycpp::chalk::ansi::*` constexpr literals and inline helpers. No companion repo, no companion library reuse — the surface is small, package-specific, and not yet shared.
- vendored `supports-color`: re-implement as `polycpp::chalk::detectColorSupport(fd)` and friends. Use `polycpp::process::getenv` plus the platform `isatty` call. Keep the env-variable ladder identical to upstream so downstream behavior matches `test/no-color-support.js` and `test/level.js`.
- chalk public surface: re-implement as `polycpp::chalk::Chalk` with explicit chain methods (`red()`, `bold()`, ...) and a private `Styler` linked list. Replace JavaScript Symbols / `Object.defineProperty` memoization with a `std::shared_ptr<const Styler>` chain. Map JavaScript metaprogramming to typed C++ APIs.
- multi-arg call: implement `Chalk::operator()` overloads with explicit string-conversion semantics; document the conversion policy in `docs/api-mapping.md` because JavaScript coercion does not have a one-to-one C++ equivalent.
- level validation: throw `polycpp::Error` to keep error-handling parity with other companion libraries (`content-type`, `iconv-lite`).

Each porting decision is consistent with the ecosystem reuse decisions
recorded in `docs/research.md`.

## Analyzer warnings

- none emitted by analyzer
