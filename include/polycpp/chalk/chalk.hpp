#pragma once

/**
 * @file chalk.hpp
 * @brief Terminal string styling with ANSI escape codes.
 *
 * C++ port of npm chalk (https://github.com/chalk/chalk). Provides
 * a chainable Chalk class plus auto-detected global stdout/stderr
 * instances, modifier and color chain methods, level-aware RGB / hex /
 * ANSI 256 dispatch, and the same modifier/foreground/background/color
 * name arrays the upstream package exports.
 *
 * @see https://www.npmjs.com/package/chalk
 * @since 1.0.0
 */

#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

namespace polycpp {
namespace chalk {

/**
 * @brief Options for creating a Chalk instance.
 * @since 1.0.0
 */
struct Options {
    std::optional<int> level;  ///< Color support level override (0-3); when unset, falls back to supportsColor().level
};

/**
 * @brief Internal style state forming a linked list.
 *
 * Each Styler holds the open/close ANSI codes for one style layer,
 * plus accumulated openAll/closeAll strings for the entire chain.
 *
 * @since 1.0.0
 */
struct Styler {
    std::string open;                          ///< This style's ANSI open code
    std::string close;                         ///< This style's ANSI close code
    std::string openAll;                       ///< Accumulated open codes (all ancestors + this)
    std::string closeAll;                      ///< Accumulated close codes (this + all ancestors)
    std::shared_ptr<const Styler> parent;      ///< Previous style in chain
};

/**
 * @brief Terminal string styling with ANSI escape codes.
 *
 * C++ port of npm chalk. Provides chainable style methods that
 * wrap text in ANSI escape codes for terminal coloring.
 *
 * @par Example
 * @code
 * Chalk c(Options{.level = 1});
 * std::cout << c.red().bold()("Error!") << std::endl;
 * auto warning = c.yellow().underline();
 * std::cout << warning("Watch out!") << std::endl;
 * @endcode
 *
 * @see https://www.npmjs.com/package/chalk
 * @since 1.0.0
 */
class Chalk {
public:
    /**
     * @brief Default constructor; level falls back to supportsColor().level.
     * @since 1.0.0
     */
    Chalk();

    /**
     * @brief Construct with explicit options.
     * @param options Configuration options (level override). When level is
     *                unset, falls back to supportsColor().level for parity
     *                with upstream `applyOptions`.
     * @since 1.0.0
     */
    explicit Chalk(const Options& options);

    /**
     * @brief Get the current color support level.
     * @return Color level: 0=none, 1=basic 16, 2=ANSI 256, 3=truecolor.
     * @since 1.0.0
     */
    int level() const;

    /**
     * @brief Set the color support level.
     *
     * Chain links built from this Chalk share the level cell, so calling
     * setLevel propagates lazily to every previously built chain.
     *
     * @param level Color level (0-3).
     * @since 1.0.0
     */
    void setLevel(int level);

    // ─── Rendering ────────────────────────────────

    /**
     * @brief Apply accumulated styles to text.
     * @param text The text to style.
     * @return Styled text with ANSI escape codes, or plain text if level is 0.
     * @since 1.0.0
     */
    std::string operator()(const std::string& text) const;

    /**
     * @brief Apply accumulated styles to multiple arguments joined with spaces.
     * @param first First argument.
     * @param rest Remaining arguments.
     * @return Styled text with ANSI escape codes.
     * @since 1.0.0
     */
    template<typename T, typename... Args>
    std::string operator()(const T& first, const Args&... rest) const;

    // ─── Modifiers ────────────────────────────────

    /** @brief Reset all styles. @since 1.0.0 */
    Chalk reset() const;
    /** @brief Bold text. @since 1.0.0 */
    Chalk bold() const;
    /** @brief Dim (faint) text. @since 1.0.0 */
    Chalk dim() const;
    /** @brief Italic text. @since 1.0.0 */
    Chalk italic() const;
    /** @brief Underlined text. @since 1.0.0 */
    Chalk underline() const;
    /** @brief Overlined text. @since 1.0.0 */
    Chalk overline() const;
    /** @brief Inverse foreground and background colors. @since 1.0.0 */
    Chalk inverse() const;
    /** @brief Hidden (invisible) text. @since 1.0.0 */
    Chalk hidden() const;
    /** @brief Strikethrough text. @since 1.0.0 */
    Chalk strikethrough() const;

    /** @brief Only renders when level > 0; returns "" when level == 0. @since 1.0.0 */
    Chalk visible() const;

    // ─── Foreground Colors ────────────────────────

    /** @brief Foreground black (ANSI 30). @since 1.0.0 */
    Chalk black() const;
    /** @brief Foreground red (ANSI 31). @since 1.0.0 */
    Chalk red() const;
    /** @brief Foreground green (ANSI 32). @since 1.0.0 */
    Chalk green() const;
    /** @brief Foreground yellow (ANSI 33). @since 1.0.0 */
    Chalk yellow() const;
    /** @brief Foreground blue (ANSI 34). @since 1.0.0 */
    Chalk blue() const;
    /** @brief Foreground magenta (ANSI 35). @since 1.0.0 */
    Chalk magenta() const;
    /** @brief Foreground cyan (ANSI 36). @since 1.0.0 */
    Chalk cyan() const;
    /** @brief Foreground white (ANSI 37). @since 1.0.0 */
    Chalk white() const;
    /** @brief Foreground bright black (ANSI 90). @since 1.0.0 */
    Chalk blackBright() const;
    /** @brief Foreground bright red (ANSI 91). @since 1.0.0 */
    Chalk redBright() const;
    /** @brief Foreground bright green (ANSI 92). @since 1.0.0 */
    Chalk greenBright() const;
    /** @brief Foreground bright yellow (ANSI 93). @since 1.0.0 */
    Chalk yellowBright() const;
    /** @brief Foreground bright blue (ANSI 94). @since 1.0.0 */
    Chalk blueBright() const;
    /** @brief Foreground bright magenta (ANSI 95). @since 1.0.0 */
    Chalk magentaBright() const;
    /** @brief Foreground bright cyan (ANSI 96). @since 1.0.0 */
    Chalk cyanBright() const;
    /** @brief Foreground bright white (ANSI 97). @since 1.0.0 */
    Chalk whiteBright() const;
    /** @brief Alias for blackBright(). @since 1.0.0 */
    Chalk gray() const;
    /** @brief Alias for blackBright(). @since 1.0.0 */
    Chalk grey() const;

    // ─── Background Colors ────────────────────────

    /** @brief Background black (ANSI 40). @since 1.0.0 */
    Chalk bgBlack() const;
    /** @brief Background red (ANSI 41). @since 1.0.0 */
    Chalk bgRed() const;
    /** @brief Background green (ANSI 42). @since 1.0.0 */
    Chalk bgGreen() const;
    /** @brief Background yellow (ANSI 43). @since 1.0.0 */
    Chalk bgYellow() const;
    /** @brief Background blue (ANSI 44). @since 1.0.0 */
    Chalk bgBlue() const;
    /** @brief Background magenta (ANSI 45). @since 1.0.0 */
    Chalk bgMagenta() const;
    /** @brief Background cyan (ANSI 46). @since 1.0.0 */
    Chalk bgCyan() const;
    /** @brief Background white (ANSI 47). @since 1.0.0 */
    Chalk bgWhite() const;
    /** @brief Background bright black (ANSI 100). @since 1.0.0 */
    Chalk bgBlackBright() const;
    /** @brief Background bright red (ANSI 101). @since 1.0.0 */
    Chalk bgRedBright() const;
    /** @brief Background bright green (ANSI 102). @since 1.0.0 */
    Chalk bgGreenBright() const;
    /** @brief Background bright yellow (ANSI 103). @since 1.0.0 */
    Chalk bgYellowBright() const;
    /** @brief Background bright blue (ANSI 104). @since 1.0.0 */
    Chalk bgBlueBright() const;
    /** @brief Background bright magenta (ANSI 105). @since 1.0.0 */
    Chalk bgMagentaBright() const;
    /** @brief Background bright cyan (ANSI 106). @since 1.0.0 */
    Chalk bgCyanBright() const;
    /** @brief Background bright white (ANSI 107). @since 1.0.0 */
    Chalk bgWhiteBright() const;
    /** @brief Alias for bgBlackBright(). @since 1.0.0 */
    Chalk bgGray() const;
    /** @brief Alias for bgBlackBright(). @since 1.0.0 */
    Chalk bgGrey() const;

    // ─── Advanced Color Methods ───────────────────

    /**
     * @brief Set foreground color using RGB values.
     * @param r Red component (0-255).
     * @param g Green component (0-255).
     * @param b Blue component (0-255).
     * @return New Chalk instance with RGB foreground color.
     * @since 1.0.0
     */
    Chalk rgb(int r, int g, int b) const;

    /**
     * @brief Set foreground color using hex string.
     * @param color Hex color string (e.g., "#FF0000", "FF0000", "#F00").
     * @return New Chalk instance with hex foreground color.
     * @since 1.0.0
     */
    Chalk hex(const std::string& color) const;

    /**
     * @brief Set foreground color using ANSI 256 palette index.
     * @param index Palette index (0-255).
     * @return New Chalk instance with ANSI 256 foreground color.
     * @since 1.0.0
     */
    Chalk ansi256(int index) const;

    /**
     * @brief Set background color using RGB values.
     * @param r Red component (0-255).
     * @param g Green component (0-255).
     * @param b Blue component (0-255).
     * @return New Chalk instance with RGB background color.
     * @since 1.0.0
     */
    Chalk bgRgb(int r, int g, int b) const;

    /**
     * @brief Set background color using hex string.
     * @param color Hex color string (e.g., "#FF0000", "FF0000", "#F00").
     * @return New Chalk instance with hex background color.
     * @since 1.0.0
     */
    Chalk bgHex(const std::string& color) const;

    /**
     * @brief Set background color using ANSI 256 palette index.
     * @param index Palette index (0-255).
     * @return New Chalk instance with ANSI 256 background color.
     * @since 1.0.0
     */
    Chalk bgAnsi256(int index) const;

private:
    std::shared_ptr<const Styler> styler_;
    /// Shared with chain links so that mutating the parent's level
    /// (via setLevel / level()) is observed lazily by every chain
    /// link built from it. Matches upstream chalk's prototype-getter
    /// semantics in `test/level.js`.
    std::shared_ptr<int> level_ptr_;
    bool isEmpty_ = false;

    /// Create a new Chalk with an additional style layer.
    Chalk withStyle(const std::string& open, const std::string& close) const;

    /// Apply all accumulated styles to text.
    std::string applyStyle(const std::string& text) const;

    /// Private constructor for chaining (shares level_ptr with caller).
    Chalk(std::shared_ptr<const Styler> styler,
          std::shared_ptr<int> level_ptr,
          bool isEmpty);
};

// ─── Global Instances ─────────────────────────────

/**
 * @brief Pre-configured Chalk for stdout (auto-detected level).
 * @return Reference to the global stdout Chalk instance.
 * @since 1.0.0
 */
Chalk& chalk();

/**
 * @brief Pre-configured Chalk for stderr (auto-detected level).
 * @return Reference to the global stderr Chalk instance.
 * @since 1.0.0
 */
Chalk& chalkStderr();

// ─── Name Arrays ──────────────────────────────────

/**
 * @brief Get the list of modifier style names.
 * @since 1.0.0
 */
const std::vector<std::string>& modifierNames();

/**
 * @brief Get the list of foreground color names.
 * @since 1.0.0
 */
const std::vector<std::string>& foregroundColorNames();

/**
 * @brief Get the list of background color names.
 * @since 1.0.0
 */
const std::vector<std::string>& backgroundColorNames();

/**
 * @brief Get the combined list of all color names (foreground + background).
 * @since 1.0.0
 */
const std::vector<std::string>& colorNames();

} // namespace chalk
} // namespace polycpp
