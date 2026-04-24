// Print a miniature "git diff" style summary with coloured +/- markers.
//
//   $ ./diff_table
//   + added line
//   - removed line
//     unchanged line

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <polycpp/chalk/chalk.hpp>

int main() {
    using namespace polycpp::chalk;

    // Simulate a diff so the example is self-contained.
    std::vector<std::pair<char, std::string>> diff = {
        {'+', "added a greeting"},
        {'-', "removed a TODO"},
        {' ', "unchanged boilerplate"},
        {'+', "fixed typo in README"},
    };

    Chalk c(Options{.level = 2});   // deterministic output for pipes and CI
    auto added   = c.green().bold();
    auto removed = c.red().bold();
    auto context = c.dim();

    for (const auto& [marker, text] : diff) {
        switch (marker) {
            case '+': std::cout << added  ("+ " + text); break;
            case '-': std::cout << removed("- " + text); break;
            default:  std::cout << context("  " + text); break;
        }
        std::cout << '\n';
    }
    return 0;
}
