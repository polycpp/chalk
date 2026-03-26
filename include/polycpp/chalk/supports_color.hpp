#pragma once

namespace polycpp {
namespace chalk {

/// @brief Color support information for a terminal stream.
/// @see https://www.npmjs.com/package/supports-color
struct ColorSupport {
    int level;       ///< 0=none, 1=basic 16, 2=ANSI 256, 3=truecolor
    bool hasBasic;   ///< level >= 1
    bool has256;     ///< level >= 2
    bool has16m;     ///< level >= 3
};

/// @brief Detect color support for a file descriptor.
/// @param fd File descriptor (1=stdout, 2=stderr).
/// @return Detected color support information.
ColorSupport detectColorSupport(int fd);

/// @brief Cached color support for stdout.
/// @return Reference to the cached ColorSupport for stdout (fd 1).
const ColorSupport& supportsColor();

/// @brief Cached color support for stderr.
/// @return Reference to the cached ColorSupport for stderr (fd 2).
const ColorSupport& supportsColorStderr();

} // namespace chalk
} // namespace polycpp
