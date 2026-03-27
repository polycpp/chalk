#pragma once

#include <polycpp/chalk/ansi_styles.hpp>
#include <polycpp/chalk/chalk.hpp>
#include <polycpp/chalk/detail/ansi_styles.hpp>

#include <string>
#include <type_traits>
#include <utility>

namespace polycpp {
namespace chalk {

namespace detail {

/// @brief Replace every occurrence of `search` with `search + append`.
///
/// This matches the JS chalk stringReplaceAll semantics: the search string
/// is KEPT and the replacement is APPENDED after it. This is critical for
/// correct ANSI nesting behavior.
inline void replaceCloseWithReopen(std::string& str, const std::string& search, const std::string& append) {
    if (search.empty()) return;
    auto index = str.find(search);
    if (index == std::string::npos) return;

    std::string result;
    result.reserve(str.size() + append.size() * 2);  // estimate
    std::string::size_type endIndex = 0;
    do {
        result.append(str, endIndex, index - endIndex);
        result.append(search);
        result.append(append);
        endIndex = index + search.length();
        index = str.find(search, endIndex);
    } while (index != std::string::npos);

    result.append(str, endIndex, std::string::npos);
    str = std::move(result);
}

/// @brief Encase line breaks with close/open codes.
///
/// Port of stringEncaseCRLFWithFirstIndex from chalk's utilities.js.
/// Closes the style before each line break and reopens it after,
/// fixing a bleed issue on macOS terminals.
inline void encaseCRLF(std::string& str, const std::string& closeAll, const std::string& openAll, std::string::size_type firstLfIndex) {
    std::string result;
    std::string::size_type endIndex = 0;
    std::string::size_type index = firstLfIndex;

    do {
        bool gotCR = (index > 0 && str[index - 1] == '\r');
        result += str.substr(endIndex, (gotCR ? index - 1 : index) - endIndex);
        result += closeAll;
        result += (gotCR ? "\r\n" : "\n");
        result += openAll;
        endIndex = index + 1;
        index = str.find('\n', endIndex);
    } while (index != std::string::npos);

    result += str.substr(endIndex);
    str = std::move(result);
}

} // namespace detail

// ─── Constructors ─────────────────────────────────

inline Chalk::Chalk() : level_(0) {}

inline Chalk::Chalk(const Options& options) : level_(options.level.value_or(0)) {}

inline Chalk::Chalk(std::shared_ptr<const Styler> styler, int level, bool isEmpty)
    : styler_(std::move(styler)), level_(level), isEmpty_(isEmpty) {}

// ─── Accessors ────────────────────────────────────

inline int Chalk::level() const { return level_; }

inline void Chalk::setLevel(int level) { level_ = level; }

// ─── Core Implementation ──────────────────────────

inline Chalk Chalk::withStyle(const std::string& open, const std::string& close) const {
    auto s = std::make_shared<Styler>();
    s->open = open;
    s->close = close;
    if (styler_) {
        s->openAll = styler_->openAll + open;
        s->closeAll = close + styler_->closeAll;
    } else {
        s->openAll = open;
        s->closeAll = close;
    }
    s->parent = styler_;
    return Chalk(std::move(s), level_, isEmpty_);
}

inline std::string Chalk::applyStyle(const std::string& text) const {
    if (level_ <= 0 || text.empty()) {
        return isEmpty_ ? "" : text;
    }

    if (!styler_) {
        return text;
    }

    std::string result = text;

    // Handle nested ANSI codes: replace any close codes from the styler
    // chain with close+reopen, so inner styles don't break outer styles.
    if (result.find('\x1B') != std::string::npos) {
        auto s = styler_;
        while (s) {
            detail::replaceCloseWithReopen(result, s->close, s->open);
            s = s->parent;
        }
    }

    // Handle line breaks: close the style before each line break and
    // reopen it after, fixing a bleed issue on macOS terminals.
    auto lfIndex = result.find('\n');
    if (lfIndex != std::string::npos) {
        detail::encaseCRLF(result, styler_->closeAll, styler_->openAll, lfIndex);
    }

    return styler_->openAll + result + styler_->closeAll;
}

inline std::string Chalk::operator()(const std::string& text) const {
    return applyStyle(text);
}

template<typename T, typename... Args>
std::string Chalk::operator()(const T& first, const Args&... rest) const {
    auto toString = [](const auto& val) -> std::string {
        if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::string>) {
            return val;
        } else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, const char*> ||
                             std::is_same_v<std::decay_t<decltype(val)>, char*>) {
            return std::string(val);
        } else if constexpr (std::is_arithmetic_v<std::decay_t<decltype(val)>>) {
            return std::to_string(val);
        } else {
            return std::string(val);
        }
    };

    std::string joined = toString(first);
    ((joined += " " + toString(rest)), ...);
    return applyStyle(joined);
}

// ─── Modifiers ────────────────────────────────────

inline Chalk Chalk::reset() const {
    return withStyle(ansi::wrapAnsi16(ansi::RESET.open), ansi::wrapAnsi16(ansi::RESET.close));
}

inline Chalk Chalk::bold() const {
    return withStyle(ansi::wrapAnsi16(ansi::BOLD.open), ansi::wrapAnsi16(ansi::BOLD.close));
}

inline Chalk Chalk::dim() const {
    return withStyle(ansi::wrapAnsi16(ansi::DIM.open), ansi::wrapAnsi16(ansi::DIM.close));
}

inline Chalk Chalk::italic() const {
    return withStyle(ansi::wrapAnsi16(ansi::ITALIC.open), ansi::wrapAnsi16(ansi::ITALIC.close));
}

inline Chalk Chalk::underline() const {
    return withStyle(ansi::wrapAnsi16(ansi::UNDERLINE.open), ansi::wrapAnsi16(ansi::UNDERLINE.close));
}

inline Chalk Chalk::overline() const {
    return withStyle(ansi::wrapAnsi16(ansi::OVERLINE.open), ansi::wrapAnsi16(ansi::OVERLINE.close));
}

inline Chalk Chalk::inverse() const {
    return withStyle(ansi::wrapAnsi16(ansi::INVERSE.open), ansi::wrapAnsi16(ansi::INVERSE.close));
}

inline Chalk Chalk::hidden() const {
    return withStyle(ansi::wrapAnsi16(ansi::HIDDEN.open), ansi::wrapAnsi16(ansi::HIDDEN.close));
}

inline Chalk Chalk::strikethrough() const {
    return withStyle(ansi::wrapAnsi16(ansi::STRIKETHROUGH.open), ansi::wrapAnsi16(ansi::STRIKETHROUGH.close));
}

inline Chalk Chalk::visible() const {
    return Chalk(styler_, level_, true);
}

// ─── Foreground Colors ────────────────────────────

inline Chalk Chalk::black() const {
    return withStyle(ansi::wrapAnsi16(ansi::BLACK.open), ansi::wrapAnsi16(ansi::BLACK.close));
}

inline Chalk Chalk::red() const {
    return withStyle(ansi::wrapAnsi16(ansi::RED.open), ansi::wrapAnsi16(ansi::RED.close));
}

inline Chalk Chalk::green() const {
    return withStyle(ansi::wrapAnsi16(ansi::GREEN.open), ansi::wrapAnsi16(ansi::GREEN.close));
}

inline Chalk Chalk::yellow() const {
    return withStyle(ansi::wrapAnsi16(ansi::YELLOW.open), ansi::wrapAnsi16(ansi::YELLOW.close));
}

inline Chalk Chalk::blue() const {
    return withStyle(ansi::wrapAnsi16(ansi::BLUE.open), ansi::wrapAnsi16(ansi::BLUE.close));
}

inline Chalk Chalk::magenta() const {
    return withStyle(ansi::wrapAnsi16(ansi::MAGENTA.open), ansi::wrapAnsi16(ansi::MAGENTA.close));
}

inline Chalk Chalk::cyan() const {
    return withStyle(ansi::wrapAnsi16(ansi::CYAN.open), ansi::wrapAnsi16(ansi::CYAN.close));
}

inline Chalk Chalk::white() const {
    return withStyle(ansi::wrapAnsi16(ansi::WHITE.open), ansi::wrapAnsi16(ansi::WHITE.close));
}

inline Chalk Chalk::blackBright() const {
    return withStyle(ansi::wrapAnsi16(ansi::BLACK_BRIGHT.open), ansi::wrapAnsi16(ansi::BLACK_BRIGHT.close));
}

inline Chalk Chalk::redBright() const {
    return withStyle(ansi::wrapAnsi16(ansi::RED_BRIGHT.open), ansi::wrapAnsi16(ansi::RED_BRIGHT.close));
}

inline Chalk Chalk::greenBright() const {
    return withStyle(ansi::wrapAnsi16(ansi::GREEN_BRIGHT.open), ansi::wrapAnsi16(ansi::GREEN_BRIGHT.close));
}

inline Chalk Chalk::yellowBright() const {
    return withStyle(ansi::wrapAnsi16(ansi::YELLOW_BRIGHT.open), ansi::wrapAnsi16(ansi::YELLOW_BRIGHT.close));
}

inline Chalk Chalk::blueBright() const {
    return withStyle(ansi::wrapAnsi16(ansi::BLUE_BRIGHT.open), ansi::wrapAnsi16(ansi::BLUE_BRIGHT.close));
}

inline Chalk Chalk::magentaBright() const {
    return withStyle(ansi::wrapAnsi16(ansi::MAGENTA_BRIGHT.open), ansi::wrapAnsi16(ansi::MAGENTA_BRIGHT.close));
}

inline Chalk Chalk::cyanBright() const {
    return withStyle(ansi::wrapAnsi16(ansi::CYAN_BRIGHT.open), ansi::wrapAnsi16(ansi::CYAN_BRIGHT.close));
}

inline Chalk Chalk::whiteBright() const {
    return withStyle(ansi::wrapAnsi16(ansi::WHITE_BRIGHT.open), ansi::wrapAnsi16(ansi::WHITE_BRIGHT.close));
}

inline Chalk Chalk::gray() const {
    return blackBright();
}

inline Chalk Chalk::grey() const {
    return blackBright();
}

// ─── Background Colors ────────────────────────────

inline Chalk Chalk::bgBlack() const {
    return withStyle(ansi::wrapAnsi16(ansi::BG_BLACK.open), ansi::wrapAnsi16(ansi::BG_BLACK.close));
}

inline Chalk Chalk::bgRed() const {
    return withStyle(ansi::wrapAnsi16(ansi::BG_RED.open), ansi::wrapAnsi16(ansi::BG_RED.close));
}

inline Chalk Chalk::bgGreen() const {
    return withStyle(ansi::wrapAnsi16(ansi::BG_GREEN.open), ansi::wrapAnsi16(ansi::BG_GREEN.close));
}

inline Chalk Chalk::bgYellow() const {
    return withStyle(ansi::wrapAnsi16(ansi::BG_YELLOW.open), ansi::wrapAnsi16(ansi::BG_YELLOW.close));
}

inline Chalk Chalk::bgBlue() const {
    return withStyle(ansi::wrapAnsi16(ansi::BG_BLUE.open), ansi::wrapAnsi16(ansi::BG_BLUE.close));
}

inline Chalk Chalk::bgMagenta() const {
    return withStyle(ansi::wrapAnsi16(ansi::BG_MAGENTA.open), ansi::wrapAnsi16(ansi::BG_MAGENTA.close));
}

inline Chalk Chalk::bgCyan() const {
    return withStyle(ansi::wrapAnsi16(ansi::BG_CYAN.open), ansi::wrapAnsi16(ansi::BG_CYAN.close));
}

inline Chalk Chalk::bgWhite() const {
    return withStyle(ansi::wrapAnsi16(ansi::BG_WHITE.open), ansi::wrapAnsi16(ansi::BG_WHITE.close));
}

inline Chalk Chalk::bgBlackBright() const {
    return withStyle(ansi::wrapAnsi16(ansi::BG_BLACK_BRIGHT.open), ansi::wrapAnsi16(ansi::BG_BLACK_BRIGHT.close));
}

inline Chalk Chalk::bgRedBright() const {
    return withStyle(ansi::wrapAnsi16(ansi::BG_RED_BRIGHT.open), ansi::wrapAnsi16(ansi::BG_RED_BRIGHT.close));
}

inline Chalk Chalk::bgGreenBright() const {
    return withStyle(ansi::wrapAnsi16(ansi::BG_GREEN_BRIGHT.open), ansi::wrapAnsi16(ansi::BG_GREEN_BRIGHT.close));
}

inline Chalk Chalk::bgYellowBright() const {
    return withStyle(ansi::wrapAnsi16(ansi::BG_YELLOW_BRIGHT.open), ansi::wrapAnsi16(ansi::BG_YELLOW_BRIGHT.close));
}

inline Chalk Chalk::bgBlueBright() const {
    return withStyle(ansi::wrapAnsi16(ansi::BG_BLUE_BRIGHT.open), ansi::wrapAnsi16(ansi::BG_BLUE_BRIGHT.close));
}

inline Chalk Chalk::bgMagentaBright() const {
    return withStyle(ansi::wrapAnsi16(ansi::BG_MAGENTA_BRIGHT.open), ansi::wrapAnsi16(ansi::BG_MAGENTA_BRIGHT.close));
}

inline Chalk Chalk::bgCyanBright() const {
    return withStyle(ansi::wrapAnsi16(ansi::BG_CYAN_BRIGHT.open), ansi::wrapAnsi16(ansi::BG_CYAN_BRIGHT.close));
}

inline Chalk Chalk::bgWhiteBright() const {
    return withStyle(ansi::wrapAnsi16(ansi::BG_WHITE_BRIGHT.open), ansi::wrapAnsi16(ansi::BG_WHITE_BRIGHT.close));
}

inline Chalk Chalk::bgGray() const {
    return bgBlackBright();
}

inline Chalk Chalk::bgGrey() const {
    return bgBlackBright();
}

// ─── Advanced Color Methods ───────────────────────

inline Chalk Chalk::rgb(int r, int g, int b) const {
    return withStyle(ansi::makeColorCode(level_, r, g, b, false), ansi::FG_CLOSE);
}

inline Chalk Chalk::hex(const std::string& color) const {
    auto [r, g, b] = ansi::hexToRgb(color);
    return withStyle(ansi::makeColorCode(level_, r, g, b, false), ansi::FG_CLOSE);
}

inline Chalk Chalk::ansi256(int index) const {
    return withStyle(ansi::makeColorCode256(level_, index, false), ansi::FG_CLOSE);
}

inline Chalk Chalk::bgRgb(int r, int g, int b) const {
    return withStyle(ansi::makeColorCode(level_, r, g, b, true), ansi::BG_CLOSE);
}

inline Chalk Chalk::bgHex(const std::string& color) const {
    auto [r, g, b] = ansi::hexToRgb(color);
    return withStyle(ansi::makeColorCode(level_, r, g, b, true), ansi::BG_CLOSE);
}

inline Chalk Chalk::bgAnsi256(int index) const {
    return withStyle(ansi::makeColorCode256(level_, index, true), ansi::BG_CLOSE);
}

} // namespace chalk
} // namespace polycpp
