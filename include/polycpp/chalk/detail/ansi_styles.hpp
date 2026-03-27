#pragma once

#include <polycpp/chalk/ansi_styles.hpp>

#include <polycpp/core/math.hpp>

#include <cctype>
#include <string>

namespace polycpp {
namespace chalk {
namespace ansi {

inline std::string wrapAnsi16(int code) {
    return "\x1B[" + std::to_string(code) + "m";
}

inline std::string wrapAnsi256(int code, bool background) {
    return "\x1B[" + std::to_string(background ? 48 : 38) + ";5;" + std::to_string(code) + "m";
}

inline std::string wrapAnsi16m(int r, int g, int b, bool background) {
    return "\x1B[" + std::to_string(background ? 48 : 38) + ";2;" +
           std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

inline int rgbToAnsi256(int r, int g, int b) {
    // Use the extended greyscale palette, with the exception of black and white.
    // Normal palette only has 4 greyscale shades.
    if (r == g && g == b) {
        if (r < 8) {
            return 16;
        }

        if (r > 248) {
            return 231;
        }

        return static_cast<int>(polycpp::Math::round(((r - 8) / 247.0) * 24)) + 232;
    }

    return 16
        + (36 * static_cast<int>(polycpp::Math::round(r / 255.0 * 5)))
        + (6 * static_cast<int>(polycpp::Math::round(g / 255.0 * 5)))
        + static_cast<int>(polycpp::Math::round(b / 255.0 * 5));
}

inline std::array<int, 3> hexToRgb(const std::string& hex) {
    // Find the first run of hex digits (3 or 6 chars) in the string,
    // skipping any leading '#'.
    std::string digits;
    for (char ch : hex) {
        if (std::isxdigit(static_cast<unsigned char>(ch))) {
            digits += ch;
        } else if (ch != '#') {
            // Non-hex, non-hash character breaks the sequence
            break;
        }
    }

    if (digits.length() != 3 && digits.length() != 6) {
        return {0, 0, 0};
    }

    // Expand 3-char shorthand: "F0A" -> "FF00AA"
    if (digits.length() == 3) {
        std::string expanded;
        for (char ch : digits) {
            expanded += ch;
            expanded += ch;
        }
        digits = expanded;
    }

    // Parse the 6-char hex integer
    unsigned long integer = 0;
    for (char ch : digits) {
        integer <<= 4;
        if (ch >= '0' && ch <= '9') {
            integer |= (ch - '0');
        } else if (ch >= 'a' && ch <= 'f') {
            integer |= (ch - 'a' + 10);
        } else if (ch >= 'A' && ch <= 'F') {
            integer |= (ch - 'A' + 10);
        }
    }

    return {
        static_cast<int>((integer >> 16) & 0xFF),
        static_cast<int>((integer >> 8) & 0xFF),
        static_cast<int>(integer & 0xFF)
    };
}

inline int ansi256ToAnsi(int code) {
    if (code < 8) {
        return 30 + code;
    }

    if (code < 16) {
        return 90 + (code - 8);
    }

    double red;
    double green;
    double blue;

    if (code >= 232) {
        red = green = blue = (((code - 232) * 10) + 8) / 255.0;
    } else {
        int c = code - 16;
        int remainder = c % 36;
        red = polycpp::Math::floor(c / 36.0) / 5.0;
        green = polycpp::Math::floor(remainder / 6.0) / 5.0;
        blue = (remainder % 6) / 5.0;
    }

    double value = polycpp::Math::max(red, green, blue) * 2;

    if (value == 0) {
        return 30;
    }

    int result = 30 + ((static_cast<int>(polycpp::Math::round(blue)) << 2) |
                        (static_cast<int>(polycpp::Math::round(green)) << 1) |
                        static_cast<int>(polycpp::Math::round(red)));

    if (value == 2) {
        result += 60;
    }

    return result;
}

inline int hexToAnsi256(const std::string& hex) {
    auto [r, g, b] = hexToRgb(hex);
    return rgbToAnsi256(r, g, b);
}

inline int rgbToAnsi(int r, int g, int b) {
    return ansi256ToAnsi(rgbToAnsi256(r, g, b));
}

inline std::string makeColorCode(int level, int r, int g, int b, bool background) {
    if (level == 3) {
        return wrapAnsi16m(r, g, b, background);
    }

    if (level == 2) {
        return wrapAnsi256(rgbToAnsi256(r, g, b), background);
    }

    // Level 1: basic 16 colors
    // rgbToAnsi returns 30-37 or 90-97 (foreground codes).
    // For background, add 10 to get 40-47 or 100-107.
    return wrapAnsi16(rgbToAnsi(r, g, b) + (background ? 10 : 0));
}

inline std::string makeColorCode(int level, const std::string& hex, bool background) {
    auto [r, g, b] = hexToRgb(hex);
    return makeColorCode(level, r, g, b, background);
}

inline std::string makeColorCode256(int level, int index, bool background) {
    // JS chalk always outputs 256-color format for ansi256(), regardless of
    // terminal level. The getModelAnsi function (index.js:91) falls through to
    // ansiStyles[type][model]() which always uses wrapAnsi256. No downsampling.
    (void)level;
    return wrapAnsi256(index, background);
}

} // namespace ansi
} // namespace chalk
} // namespace polycpp
