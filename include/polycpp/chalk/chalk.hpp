#pragma once

#include <string>
#include <optional>

namespace polycpp {
namespace chalk {

/// @brief Options for creating a Chalk instance.
struct Options {
    std::optional<int> level;  ///< Color support level override (0-3)
};

/// @brief Terminal string styling with ANSI escape codes.
///
/// C++ port of npm chalk. Provides chainable style methods that
/// wrap text in ANSI escape codes for terminal coloring.
///
/// @par Example
/// @code
/// Chalk c(Options{.level = 1});
/// std::cout << c.red().bold()("Error!") << std::endl;
/// @endcode
///
/// @see https://www.npmjs.com/package/chalk
class Chalk {
public:
    /// @brief Construct with default color level (0).
    Chalk();

    /// @brief Construct with explicit options.
    /// @param options Configuration options (level override).
    explicit Chalk(const Options& options);

    /// @brief Get the current color support level.
    /// @return Color level: 0=none, 1=basic 16, 2=ANSI 256, 3=truecolor.
    int level() const;

    /// @brief Set the color support level.
    /// @param level Color level (0-3).
    void setLevel(int level);

    /// @brief Apply accumulated styles to text.
    /// @param text The text to style.
    /// @return Styled text with ANSI escape codes, or plain text if level is 0.
    std::string operator()(const std::string& text) const;

private:
    int level_;
};

} // namespace chalk
} // namespace polycpp
