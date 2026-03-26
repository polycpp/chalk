# polycpp/chalk

C++ port of [chalk](https://www.npmjs.com/package/chalk) — terminal string styling done right.

Part of the [polycpp](https://github.com/enricohuang/polycpp) companion library ecosystem.

## Features

- Chainable style API: `chalk.red().bold()("Error!")`
- 16, 256, and truecolor (16 million) color support
- Auto-detects terminal color capabilities
- Proper handling of nested styles and line breaks
- RGB, hex, and ANSI 256 color methods

## Prerequisites

- C++20 compiler (GCC 13+, Clang 16+)
- CMake 3.20+
- Ninja (recommended)

## Build

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
cd build && ctest --output-on-failure
```

## Usage

```cpp
#include <polycpp/chalk/chalk.hpp>
#include <iostream>

using polycpp::chalk::Chalk;
using polycpp::chalk::Options;

int main() {
    Chalk chalk(Options{.level = 3});

    // Basic styling
    std::cout << chalk.red()("Error!") << std::endl;
    std::cout << chalk.green().bold()("Success!") << std::endl;

    // Reusable themes
    auto error = chalk.bold().red();
    auto warning = chalk.hex("#FFA500");
    std::cout << error("Something failed") << std::endl;
    std::cout << warning("Be careful") << std::endl;

    return 0;
}
```

## License

MIT — see [LICENSE](LICENSE).
