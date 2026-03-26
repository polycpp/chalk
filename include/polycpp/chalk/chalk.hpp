#pragma once

#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

namespace polycpp {
namespace chalk {

/// @brief Options for creating a Chalk instance.
struct Options {
    std::optional<int> level;  ///< Color support level override (0-3)
};

/// @brief Internal style state forming a linked list.
///
/// Each Styler holds the open/close ANSI codes for one style layer,
/// plus accumulated openAll/closeAll strings for the entire chain.
struct Styler {
    std::string open;                          ///< This style's ANSI open code
    std::string close;                         ///< This style's ANSI close code
    std::string openAll;                       ///< Accumulated open codes (all ancestors + this)
    std::string closeAll;                      ///< Accumulated close codes (this + all ancestors)
    std::shared_ptr<const Styler> parent;      ///< Previous style in chain
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
/// auto warning = c.yellow().underline();
/// std::cout << warning("Watch out!") << std::endl;
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

    // ─── Rendering ────────────────────────────────

    /// @brief Apply accumulated styles to text.
    /// @param text The text to style.
    /// @return Styled text with ANSI escape codes, or plain text if level is 0.
    std::string operator()(const std::string& text) const;

    /// @brief Apply accumulated styles to multiple arguments joined with spaces.
    /// @param first First argument.
    /// @param rest Remaining arguments.
    /// @return Styled text with ANSI escape codes.
    template<typename T, typename... Args>
    std::string operator()(const T& first, const Args&... rest) const;

    // ─── Modifiers ────────────────────────────────

    /// @brief Reset all styles.
    Chalk reset() const;
    /// @brief Bold text.
    Chalk bold() const;
    /// @brief Dim (faint) text.
    Chalk dim() const;
    /// @brief Italic text.
    Chalk italic() const;
    /// @brief Underlined text.
    Chalk underline() const;
    /// @brief Overlined text.
    Chalk overline() const;
    /// @brief Inverse foreground and background colors.
    Chalk inverse() const;
    /// @brief Hidden (invisible) text.
    Chalk hidden() const;
    /// @brief Strikethrough text.
    Chalk strikethrough() const;

    /// @brief Only renders when level > 0; returns "" when level == 0.
    Chalk visible() const;

    // ─── Foreground Colors ────────────────────────

    Chalk black() const;
    Chalk red() const;
    Chalk green() const;
    Chalk yellow() const;
    Chalk blue() const;
    Chalk magenta() const;
    Chalk cyan() const;
    Chalk white() const;
    Chalk blackBright() const;
    Chalk redBright() const;
    Chalk greenBright() const;
    Chalk yellowBright() const;
    Chalk blueBright() const;
    Chalk magentaBright() const;
    Chalk cyanBright() const;
    Chalk whiteBright() const;
    Chalk gray() const;     ///< Alias for blackBright()
    Chalk grey() const;     ///< Alias for blackBright()

    // ─── Background Colors ────────────────────────

    Chalk bgBlack() const;
    Chalk bgRed() const;
    Chalk bgGreen() const;
    Chalk bgYellow() const;
    Chalk bgBlue() const;
    Chalk bgMagenta() const;
    Chalk bgCyan() const;
    Chalk bgWhite() const;
    Chalk bgBlackBright() const;
    Chalk bgRedBright() const;
    Chalk bgGreenBright() const;
    Chalk bgYellowBright() const;
    Chalk bgBlueBright() const;
    Chalk bgMagentaBright() const;
    Chalk bgCyanBright() const;
    Chalk bgWhiteBright() const;
    Chalk bgGray() const;   ///< Alias for bgBlackBright()
    Chalk bgGrey() const;   ///< Alias for bgBlackBright()

    // ─── Advanced Color Methods ───────────────────

    /// @brief Set foreground color using RGB values.
    /// @param r Red component (0-255).
    /// @param g Green component (0-255).
    /// @param b Blue component (0-255).
    /// @return New Chalk instance with RGB foreground color.
    Chalk rgb(int r, int g, int b) const;

    /// @brief Set foreground color using hex string.
    /// @param color Hex color string (e.g., "#FF0000", "FF0000", "#F00").
    /// @return New Chalk instance with hex foreground color.
    Chalk hex(const std::string& color) const;

    /// @brief Set foreground color using ANSI 256 palette index.
    /// @param index Palette index (0-255).
    /// @return New Chalk instance with ANSI 256 foreground color.
    Chalk ansi256(int index) const;

    /// @brief Set background color using RGB values.
    /// @param r Red component (0-255).
    /// @param g Green component (0-255).
    /// @param b Blue component (0-255).
    /// @return New Chalk instance with RGB background color.
    Chalk bgRgb(int r, int g, int b) const;

    /// @brief Set background color using hex string.
    /// @param color Hex color string (e.g., "#FF0000", "FF0000", "#F00").
    /// @return New Chalk instance with hex background color.
    Chalk bgHex(const std::string& color) const;

    /// @brief Set background color using ANSI 256 palette index.
    /// @param index Palette index (0-255).
    /// @return New Chalk instance with ANSI 256 background color.
    Chalk bgAnsi256(int index) const;

private:
    std::shared_ptr<const Styler> styler_;
    int level_;
    bool isEmpty_ = false;

    /// Create a new Chalk with an additional style layer.
    Chalk withStyle(const std::string& open, const std::string& close) const;

    /// Apply all accumulated styles to text.
    std::string applyStyle(const std::string& text) const;

    /// Private constructor for chaining (all fields).
    Chalk(std::shared_ptr<const Styler> styler, int level, bool isEmpty);
};

// ─── Global Instances ─────────────────────────────

/// @brief Pre-configured Chalk for stdout (auto-detected level).
/// @return Reference to the global stdout Chalk instance.
Chalk& chalk();

/// @brief Pre-configured Chalk for stderr (auto-detected level).
/// @return Reference to the global stderr Chalk instance.
Chalk& chalkStderr();

// ─── Name Arrays ──────────────────────────────────

/// @brief Get the list of modifier style names.
const std::vector<std::string>& modifierNames();

/// @brief Get the list of foreground color names.
const std::vector<std::string>& foregroundColorNames();

/// @brief Get the list of background color names.
const std::vector<std::string>& backgroundColorNames();

/// @brief Get the combined list of all color names (foreground + background).
const std::vector<std::string>& colorNames();

} // namespace chalk
} // namespace polycpp
