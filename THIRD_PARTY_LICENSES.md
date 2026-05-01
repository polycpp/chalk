# Third-Party Licenses

## Upstream package: chalk

- Source: https://github.com/chalk/chalk
- Version basis: 5.6.2
- License: MIT
- License evidence: upstream `package.json` and `license` file
- Use in this repo: clean-room C++ port based on public behavior, public TypeScript declarations, and ported tests; no upstream source files are vendored

```
MIT License

Copyright (c) Sindre Sorhus <sindresorhus@gmail.com> (https://sindresorhus.com)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```

## Vendored runtime dependencies (inside upstream chalk 5 tarball)

These two packages are vendored inside the chalk 5 npm tarball under
`source/vendor/`. The C++ port re-implements their behavior from scratch
rather than vendoring upstream source, so their MIT notices travel inside
the chalk MIT notice above. They are listed here for traceability.

- ansi-styles: https://github.com/chalk/ansi-styles, MIT
- supports-color: https://github.com/chalk/supports-color, MIT
