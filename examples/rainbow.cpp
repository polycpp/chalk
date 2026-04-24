// Print a short message where every character is a different hue.
//
//   $ ./rainbow 'polycpp chalk'
//   p o l y c p p   c h a l k   <-- each letter a different hue

#include <cmath>
#include <iostream>
#include <string>

#include <polycpp/chalk/chalk.hpp>

int main(int argc, char** argv) {
    using namespace polycpp::chalk;

    std::string text = (argc > 1) ? argv[1] : "polycpp chalk";

    // Force level 3 so the rainbow is visible regardless of the environment.
    Chalk c(Options{.level = 3});

    const std::size_t n = text.size();
    for (std::size_t i = 0; i < n; ++i) {
        double t = n > 1 ? static_cast<double>(i) / (n - 1) : 0.0;
        int r = static_cast<int>(std::round(127.5 * (1.0 + std::sin(6.2831853 * t + 0.0))));
        int g = static_cast<int>(std::round(127.5 * (1.0 + std::sin(6.2831853 * t + 2.0944))));
        int b = static_cast<int>(std::round(127.5 * (1.0 + std::sin(6.2831853 * t + 4.1887))));
        std::cout << c.rgb(r, g, b).bold()(std::string(1, text[i]));
    }
    std::cout << '\n';
    return 0;
}
