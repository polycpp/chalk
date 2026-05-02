#pragma once

/**
 * @file ansi_styles.hpp
 * @brief ANSI escape code constants, formatters, and color conversions.
 *
 * Public surface mirrors the upstream `ansi-styles` package vendored
 * inside chalk 5: open/close code pairs for every modifier and color,
 * `wrapAnsi16/256/16m` formatters, RGB-to-ANSI256 conversion math, hex
 * parsing, and level-aware `makeColorCode` dispatch.
 *
 * @see https://github.com/chalk/ansi-styles
 * @since 1.0.0
 */

#include <array>
#include <string>

namespace polycpp {
namespace chalk {
namespace ansi {

/**
 * @brief ANSI code pair: open code number and close code number.
 * @since 1.0.0
 */
struct AnsiCode {
    int open;
    int close;
};

// ─── Modifier Codes ───────────────────────────────
/** @brief Reset (CSI 0). @since 1.0.0 */
constexpr AnsiCode RESET         = {0, 0};
/** @brief Bold (CSI 1, close 22). @since 1.0.0 */
constexpr AnsiCode BOLD          = {1, 22};
/** @brief Dim/faint (CSI 2, close 22). @since 1.0.0 */
constexpr AnsiCode DIM           = {2, 22};
/** @brief Italic (CSI 3, close 23). @since 1.0.0 */
constexpr AnsiCode ITALIC        = {3, 23};
/** @brief Underline (CSI 4, close 24). @since 1.0.0 */
constexpr AnsiCode UNDERLINE     = {4, 24};
/** @brief Overline (CSI 53, close 55). @since 1.0.0 */
constexpr AnsiCode OVERLINE      = {53, 55};
/** @brief Inverse (CSI 7, close 27). @since 1.0.0 */
constexpr AnsiCode INVERSE       = {7, 27};
/** @brief Hidden (CSI 8, close 28). @since 1.0.0 */
constexpr AnsiCode HIDDEN        = {8, 28};
/** @brief Strikethrough (CSI 9, close 29). @since 1.0.0 */
constexpr AnsiCode STRIKETHROUGH = {9, 29};

// ─── Foreground Color Codes (close = 39) ──────────
/** @brief Foreground black. @since 1.0.0 */
constexpr AnsiCode BLACK          = {30, 39};
/** @brief Foreground red. @since 1.0.0 */
constexpr AnsiCode RED            = {31, 39};
/** @brief Foreground green. @since 1.0.0 */
constexpr AnsiCode GREEN          = {32, 39};
/** @brief Foreground yellow. @since 1.0.0 */
constexpr AnsiCode YELLOW         = {33, 39};
/** @brief Foreground blue. @since 1.0.0 */
constexpr AnsiCode BLUE           = {34, 39};
/** @brief Foreground magenta. @since 1.0.0 */
constexpr AnsiCode MAGENTA        = {35, 39};
/** @brief Foreground cyan. @since 1.0.0 */
constexpr AnsiCode CYAN           = {36, 39};
/** @brief Foreground white. @since 1.0.0 */
constexpr AnsiCode WHITE          = {37, 39};
/** @brief Foreground bright black. @since 1.0.0 */
constexpr AnsiCode BLACK_BRIGHT   = {90, 39};
/** @brief Foreground bright red. @since 1.0.0 */
constexpr AnsiCode RED_BRIGHT     = {91, 39};
/** @brief Foreground bright green. @since 1.0.0 */
constexpr AnsiCode GREEN_BRIGHT   = {92, 39};
/** @brief Foreground bright yellow. @since 1.0.0 */
constexpr AnsiCode YELLOW_BRIGHT  = {93, 39};
/** @brief Foreground bright blue. @since 1.0.0 */
constexpr AnsiCode BLUE_BRIGHT    = {94, 39};
/** @brief Foreground bright magenta. @since 1.0.0 */
constexpr AnsiCode MAGENTA_BRIGHT = {95, 39};
/** @brief Foreground bright cyan. @since 1.0.0 */
constexpr AnsiCode CYAN_BRIGHT    = {96, 39};
/** @brief Foreground bright white. @since 1.0.0 */
constexpr AnsiCode WHITE_BRIGHT   = {97, 39};

// ─── Background Color Codes (close = 49) ─────────
/** @brief Background black. @since 1.0.0 */
constexpr AnsiCode BG_BLACK          = {40, 49};
/** @brief Background red. @since 1.0.0 */
constexpr AnsiCode BG_RED            = {41, 49};
/** @brief Background green. @since 1.0.0 */
constexpr AnsiCode BG_GREEN          = {42, 49};
/** @brief Background yellow. @since 1.0.0 */
constexpr AnsiCode BG_YELLOW         = {43, 49};
/** @brief Background blue. @since 1.0.0 */
constexpr AnsiCode BG_BLUE            = {44, 49};
/** @brief Background magenta. @since 1.0.0 */
constexpr AnsiCode BG_MAGENTA        = {45, 49};
/** @brief Background cyan. @since 1.0.0 */
constexpr AnsiCode BG_CYAN            = {46, 49};
/** @brief Background white. @since 1.0.0 */
constexpr AnsiCode BG_WHITE           = {47, 49};
/** @brief Background bright black. @since 1.0.0 */
constexpr AnsiCode BG_BLACK_BRIGHT   = {100, 49};
/** @brief Background bright red. @since 1.0.0 */
constexpr AnsiCode BG_RED_BRIGHT     = {101, 49};
/** @brief Background bright green. @since 1.0.0 */
constexpr AnsiCode BG_GREEN_BRIGHT   = {102, 49};
/** @brief Background bright yellow. @since 1.0.0 */
constexpr AnsiCode BG_YELLOW_BRIGHT  = {103, 49};
/** @brief Background bright blue. @since 1.0.0 */
constexpr AnsiCode BG_BLUE_BRIGHT    = {104, 49};
/** @brief Background bright magenta. @since 1.0.0 */
constexpr AnsiCode BG_MAGENTA_BRIGHT = {105, 49};
/** @brief Background bright cyan. @since 1.0.0 */
constexpr AnsiCode BG_CYAN_BRIGHT    = {106, 49};
/** @brief Background bright white. @since 1.0.0 */
constexpr AnsiCode BG_WHITE_BRIGHT   = {107, 49};

// ─── Group-Level Close Codes ──────────────────────
/** @brief Foreground close sequence (CSI 39). @since 1.0.0 */
constexpr const char* FG_CLOSE = "\x1B[39m";
/** @brief Background close sequence (CSI 49). @since 1.0.0 */
constexpr const char* BG_CLOSE = "\x1B[49m";

// ─── ANSI Code Formatting ─────────────────────────

/**
 * @brief Format basic ANSI code: "\x1B[{code}m"
 * @param code The ANSI code number.
 * @return Formatted ANSI escape sequence.
 * @since 1.0.0
 */
std::string wrapAnsi16(int code);

/**
 * @brief Format 256-color: "\x1B[38;5;{n}m" (fg) or "\x1B[48;5;{n}m" (bg)
 * @param code The 256-color palette index.
 * @param background If true, format as background color.
 * @return Formatted ANSI escape sequence.
 * @since 1.0.0
 */
std::string wrapAnsi256(int code, bool background = false);

/**
 * @brief Format truecolor: "\x1B[38;2;r;g;bm" (fg) or "\x1B[48;2;r;g;bm" (bg)
 * @param r Red component (0-255).
 * @param g Green component (0-255).
 * @param b Blue component (0-255).
 * @param background If true, format as background color.
 * @return Formatted ANSI escape sequence.
 * @since 1.0.0
 */
std::string wrapAnsi16m(int r, int g, int b, bool background = false);

// ─── Color Conversion ─────────────────────────────

/**
 * @brief Convert RGB to ANSI 256 palette index (16-255).
 * @param r Red component (0-255).
 * @param g Green component (0-255).
 * @param b Blue component (0-255).
 * @return ANSI 256 palette index.
 * @see https://github.com/Qix-/color-convert
 * @since 1.0.0
 */
int rgbToAnsi256(int r, int g, int b);

/**
 * @brief Convert hex string to RGB components.
 * @param hex Hex color string. Accepts "#RRGGBB", "RRGGBB", "#RGB", "RGB".
 * @return Array of {r, g, b}. Returns {0, 0, 0} on invalid input.
 * @since 1.0.0
 */
std::array<int, 3> hexToRgb(const std::string& hex);

/**
 * @brief Convert ANSI 256 index to ANSI 16 code (30-37 or 90-97).
 * @param code ANSI 256 palette index.
 * @return ANSI 16 color code.
 * @since 1.0.0
 */
int ansi256ToAnsi(int code);

/**
 * @brief Convenience: hex -> ANSI 256 index.
 * @param hex Hex color string.
 * @return ANSI 256 palette index.
 * @since 1.0.0
 */
int hexToAnsi256(const std::string& hex);

/**
 * @brief Convenience: RGB -> ANSI 16 code.
 * @param r Red component (0-255).
 * @param g Green component (0-255).
 * @param b Blue component (0-255).
 * @return ANSI 16 color code.
 * @since 1.0.0
 */
int rgbToAnsi(int r, int g, int b);

/**
 * @brief Generate ANSI open code appropriate for the given level.
 * @param level 1=basic16, 2=ansi256, 3=truecolor.
 * @param r Red component (0-255).
 * @param g Green component (0-255).
 * @param b Blue component (0-255).
 * @param background If true, generate background color code.
 * @return Formatted ANSI escape sequence.
 * @since 1.0.0
 */
std::string makeColorCode(int level, int r, int g, int b, bool background = false);

/**
 * @brief Generate ANSI open code from hex, appropriate for the given level.
 * @param level 1=basic16, 2=ansi256, 3=truecolor.
 * @param hex Hex color string.
 * @param background If true, generate background color code.
 * @return Formatted ANSI escape sequence.
 * @since 1.0.0
 */
std::string makeColorCode(int level, const std::string& hex, bool background = false);

/**
 * @brief Generate ANSI open code from 256-color index, appropriate for the given level.
 * @param level 1=basic16, 2=ansi256, 3=truecolor.
 * @param index ANSI 256 palette index.
 * @param background If true, generate background color code.
 * @return Formatted ANSI escape sequence.
 * @since 1.0.0
 */
std::string makeColorCode256(int level, int index, bool background = false);

} // namespace ansi
} // namespace chalk
} // namespace polycpp
