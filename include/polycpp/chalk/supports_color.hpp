#pragma once

/**
 * @file supports_color.hpp
 * @brief Terminal color capability detection.
 *
 * Mirrors the upstream `supports-color` package vendored inside
 * chalk 5: detects the highest color level (none / basic 16 / ANSI 256 /
 * truecolor) the terminal attached to a given file descriptor will
 * understand. Reads `FORCE_COLOR`, `NO_COLOR`, `TERM`, `COLORTERM`, CI
 * vendor markers, `TERM_PROGRAM`, `TEAMCITY_VERSION`, and (on Windows)
 * the kernel build number via `polycpp::os::release()`.
 *
 * @see https://github.com/chalk/supports-color
 * @since 1.0.0
 */

namespace polycpp {
namespace chalk {

/**
 * @brief Color support information for a terminal stream.
 * @see https://www.npmjs.com/package/supports-color
 * @since 1.0.0
 */
struct ColorSupport {
    int level;       ///< 0=none, 1=basic 16, 2=ANSI 256, 3=truecolor
    bool hasBasic;   ///< level >= 1
    bool has256;     ///< level >= 2
    bool has16m;     ///< level >= 3
};

/**
 * @brief Detect color support for a file descriptor.
 * @param fd File descriptor (1=stdout, 2=stderr).
 * @return Detected color support information.
 * @since 1.0.0
 */
ColorSupport detectColorSupport(int fd);

/**
 * @brief Cached color support for stdout.
 * @return Reference to the cached ColorSupport for stdout (fd 1).
 * @since 1.0.0
 */
const ColorSupport& supportsColor();

/**
 * @brief Cached color support for stderr.
 * @return Reference to the cached ColorSupport for stderr (fd 2).
 * @since 1.0.0
 */
const ColorSupport& supportsColorStderr();

} // namespace chalk
} // namespace polycpp
