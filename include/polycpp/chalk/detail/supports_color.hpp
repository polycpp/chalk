#pragma once

#include <polycpp/chalk/supports_color.hpp>

#include <polycpp/core/number.hpp>
#include <polycpp/platform/console.hpp>
#include <polycpp/process.hpp>

#ifdef _WIN32
#include <polycpp/os/os.hpp>
#endif

#include <cstdlib>
#include <cstring>
#include <string>

namespace polycpp {
namespace chalk {

namespace detail {

/// @brief Check if a string ends with a given suffix (case-insensitive).
inline bool endsWithCI(const std::string& str, const std::string& suffix) {
    if (suffix.size() > str.size()) return false;
    for (std::size_t i = 0; i < suffix.size(); ++i) {
        char a = str[str.size() - suffix.size() + i];
        char b = suffix[i];
        if (std::tolower(static_cast<unsigned char>(a)) != std::tolower(static_cast<unsigned char>(b))) {
            return false;
        }
    }
    return true;
}

/// @brief Check if a string starts with a given prefix (case-insensitive).
inline bool startsWithCI(const std::string& str, const std::string& prefix) {
    if (prefix.size() > str.size()) return false;
    for (std::size_t i = 0; i < prefix.size(); ++i) {
        char a = str[i];
        char b = prefix[i];
        if (std::tolower(static_cast<unsigned char>(a)) != std::tolower(static_cast<unsigned char>(b))) {
            return false;
        }
    }
    return true;
}

/// @brief Case-insensitive substring search.
inline bool containsCI(const std::string& str, const std::string& needle) {
    if (needle.empty()) return true;
    if (needle.size() > str.size()) return false;
    for (std::size_t i = 0; i <= str.size() - needle.size(); ++i) {
        bool match = true;
        for (std::size_t j = 0; j < needle.size(); ++j) {
            if (std::tolower(static_cast<unsigned char>(str[i + j])) !=
                std::tolower(static_cast<unsigned char>(needle[j]))) {
                match = false;
                break;
            }
        }
        if (match) return true;
    }
    return false;
}

/// @brief Check if environment variable is set (exists).
inline bool hasEnv(const char* name) {
    return std::getenv(name) != nullptr;
}

} // namespace detail

inline ColorSupport detectColorSupport(int fd) {
    auto makeResult = [](int level) -> ColorSupport {
        return ColorSupport{
            .level = level,
            .hasBasic = level >= 1,
            .has256 = level >= 2,
            .has16m = level >= 3
        };
    };

    // 1. Check FORCE_COLOR env var
    int forceColorLevel = -1;  // -1 means not set
    if (detail::hasEnv("FORCE_COLOR")) {
        std::string forceColor = polycpp::process::getenv("FORCE_COLOR");
        if (forceColor == "true") {
            forceColorLevel = 1;
        } else if (forceColor == "false") {
            forceColorLevel = 0;
        } else if (forceColor.empty()) {
            forceColorLevel = 1;
        } else {
            // Parse as integer using Number::parseInt (returns NaN on failure)
            double val = polycpp::Number::parseInt(forceColor, 10);
            if (polycpp::Number::isNaN(val)) {
                forceColorLevel = 0;
            } else {
                forceColorLevel = static_cast<int>(std::min(val, 3.0));
                if (forceColorLevel < 0) forceColorLevel = 0;
            }
        }
    }

    if (forceColorLevel == 0) {
        return makeResult(0);
    }

    // 2. Check NO_COLOR env var (if set and FORCE_COLOR not set -> 0)
    if (detail::hasEnv("NO_COLOR") && forceColorLevel < 0) {
        return makeResult(0);
    }

    // Check for Azure DevOps pipelines (has to be above !streamIsTTY check)
    if (detail::hasEnv("TF_BUILD") && detail::hasEnv("AGENT_NAME")) {
        return makeResult(1);
    }

    // 3. Check if fd is a TTY (if not and no force -> 0)
    bool isTTY = polycpp::platform::isTerminal(fd);
    if (!isTTY && forceColorLevel < 0) {
        return makeResult(0);
    }

    int min = forceColorLevel > 0 ? forceColorLevel : 0;

    // Check for dumb terminal
    std::string term = polycpp::process::getenv("TERM");
    if (term == "dumb") {
        return makeResult(min);
    }

#ifdef _WIN32
    // Windows-platform branch (mirrors upstream supports-color):
    //   Windows 10 build 10586 is the first Windows release that supports 256 colors.
    //   Windows 10 build 14931 is the first release that supports 16m/TrueColor.
    // polycpp::os::release() returns the kernel version as
    // "<major>.<minor>.<build>" via RtlGetVersion (e.g. "10.0.19045"),
    // matching Node's os.release() shape on Windows. Upstream JS reads
    // parts[0] and parts[2]; we do the same.
    {
        std::string osRelease = polycpp::os::release();
        auto firstDot = osRelease.find('.');
        std::string majorStr;
        std::string buildStr;
        if (firstDot != std::string::npos) {
            majorStr = osRelease.substr(0, firstDot);
            auto secondDot = osRelease.find('.', firstDot + 1);
            if (secondDot != std::string::npos) {
                buildStr = osRelease.substr(secondDot + 1);
            }
        }
        if (!majorStr.empty() && !buildStr.empty()) {
            double major = polycpp::Number::parseInt(majorStr, 10);
            double build = polycpp::Number::parseInt(buildStr, 10);
            if (!polycpp::Number::isNaN(major) && !polycpp::Number::isNaN(build)
                    && major >= 10 && build >= 10586) {
                return makeResult(build >= 14931 ? 3 : 2);
            }
        }
        return makeResult(1);
    }
#endif

    // 4. Check CI environment
    if (detail::hasEnv("CI")) {
        if (detail::hasEnv("GITHUB_ACTIONS") || detail::hasEnv("GITEA_ACTIONS") || detail::hasEnv("CIRCLECI")) {
            return makeResult(3);
        }

        if (detail::hasEnv("TRAVIS") || detail::hasEnv("APPVEYOR") ||
            detail::hasEnv("GITLAB_CI") || detail::hasEnv("BUILDKITE") ||
            detail::hasEnv("DRONE")) {
            return makeResult(1);
        }

        if (polycpp::process::getenv("CI_NAME") == "codeship") {
            return makeResult(1);
        }

        return makeResult(min);
    }

    // 5. Check TEAMCITY_VERSION
    if (detail::hasEnv("TEAMCITY_VERSION")) {
        std::string ver = polycpp::process::getenv("TEAMCITY_VERSION");
        // Match: ^(9\.(0*[1-9]\d*)\.|\d{2,}\.)
        // i.e., version >= 9.1 supports basic colors
        if (ver.size() >= 3) {
            if (ver[0] == '9' && ver[1] == '.') {
                // Check for 9.x where x >= 1
                std::string minor;
                for (std::size_t i = 2; i < ver.size() && ver[i] != '.'; ++i) {
                    minor += ver[i];
                }
                // Strip leading zeros and check if > 0
                bool nonZero = false;
                for (char ch : minor) {
                    if (ch != '0') { nonZero = true; break; }
                }
                if (nonZero) return makeResult(1);
            } else if (ver[0] >= '1' && ver[0] <= '9') {
                // Two or more digit major version (>= 10)
                if (ver[1] >= '0' && ver[1] <= '9') {
                    return makeResult(1);
                }
            }
        }
        return makeResult(0);
    }

    // 6. Check COLORTERM
    if (polycpp::process::getenv("COLORTERM") == "truecolor") {
        return makeResult(3);
    }

    // 7. Check specific TERM values for truecolor
    if (term == "xterm-kitty" || term == "xterm-ghostty" || term == "wezterm") {
        return makeResult(3);
    }

    // 8. Check TERM_PROGRAM
    if (detail::hasEnv("TERM_PROGRAM")) {
        std::string termProgram = polycpp::process::getenv("TERM_PROGRAM");
        if (termProgram == "iTerm.app") {
            std::string verStr = polycpp::process::getenv("TERM_PROGRAM_VERSION");
            int version = 0;
            if (!verStr.empty()) {
                // Parse first component before '.'
                std::string firstPart;
                for (char ch : verStr) {
                    if (ch == '.') break;
                    firstPart += ch;
                }
                double v = polycpp::Number::parseInt(firstPart, 10);
                if (!polycpp::Number::isNaN(v)) version = static_cast<int>(v);
            }
            return makeResult(version >= 3 ? 3 : 2);
        }

        if (termProgram == "Apple_Terminal") {
            return makeResult(2);
        }
    }

    // 9. Check TERM for 256-color support: *-256color or *-256
    if (detail::endsWithCI(term, "-256color") || detail::endsWithCI(term, "-256")) {
        return makeResult(2);
    }

    // 10. Check TERM for basic color support
    // JS regex: /^screen|^xterm|^vt100|^vt220|^rxvt|color|ansi|cygwin|linux/i
    // The /i flag makes ALL parts case-insensitive, including substring matches.
    if (detail::startsWithCI(term, "screen") ||
        detail::startsWithCI(term, "xterm") ||
        detail::startsWithCI(term, "vt100") ||
        detail::startsWithCI(term, "vt220") ||
        detail::startsWithCI(term, "rxvt") ||
        detail::containsCI(term, "color") ||
        detail::containsCI(term, "ansi") ||
        detail::containsCI(term, "cygwin") ||
        detail::containsCI(term, "linux")) {
        return makeResult(1);
    }

    // 11. Check COLORTERM (any value)
    if (detail::hasEnv("COLORTERM")) {
        return makeResult(1);
    }

    return makeResult(min);
}

inline const ColorSupport& supportsColor() {
    static ColorSupport result = detectColorSupport(1);
    return result;
}

inline const ColorSupport& supportsColorStderr() {
    static ColorSupport result = detectColorSupport(2);
    return result;
}

} // namespace chalk
} // namespace polycpp
