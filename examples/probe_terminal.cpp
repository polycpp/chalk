// Print the detected colour support for stdout and stderr, plus a
// sample of each capability level. Handy inside a ``--version`` flag
// or a support-ticket template.
//
//   $ ./probe_terminal
//   stdout: level=3 (truecolor, 24-bit)
//   stderr: level=3 (truecolor, 24-bit)
//   demo:   red green blue  196 46 21  #ff0066 #00cc99

#include <iostream>
#include <string>

#include <polycpp/chalk/chalk.hpp>
#include <polycpp/chalk/supports_color.hpp>

namespace chalk_ = polycpp::chalk;

static const char* levelName(int l) {
    switch (l) {
        case 0: return "none (plain text)";
        case 1: return "basic 16 colours";
        case 2: return "ANSI-256 palette";
        case 3: return "truecolor, 24-bit";
        default: return "unknown";
    }
}

static void dump(const std::string& name, const chalk_::ColorSupport& s) {
    std::cout << name << ": level=" << s.level
              << " (" << levelName(s.level) << ")\n";
}

int main() {
    dump("stdout", chalk_::supportsColor());
    dump("stderr", chalk_::supportsColorStderr());

    chalk_::Chalk c(chalk_::Options{.level = chalk_::supportsColor().level});
    std::cout << "demo: "
              << c.red()("red") << ' '
              << c.green()("green") << ' '
              << c.blue()("blue") << "  ";

    if (chalk_::supportsColor().has256) {
        std::cout << c.ansi256(196)("196") << ' '
                  << c.ansi256(46)("46") << ' '
                  << c.ansi256(21)("21") << "  ";
    }
    if (chalk_::supportsColor().has16m) {
        std::cout << c.hex("#ff0066")("#ff0066") << ' '
                  << c.hex("#00cc99")("#00cc99");
    }
    std::cout << '\n';
    return 0;
}
