#include <polycpp/chalk/detail/aggregator.hpp>

namespace polycpp {
namespace chalk {

Chalk& chalk() {
    static Chalk instance(Options{.level = supportsColor().level});
    return instance;
}

Chalk& chalkStderr() {
    static Chalk instance(Options{.level = supportsColorStderr().level});
    return instance;
}

const std::vector<std::string>& modifierNames() {
    static const std::vector<std::string> names = {
        "reset", "bold", "dim", "italic", "underline",
        "overline", "inverse", "hidden", "strikethrough"
    };
    return names;
}

const std::vector<std::string>& foregroundColorNames() {
    static const std::vector<std::string> names = {
        "black", "red", "green", "yellow", "blue", "magenta", "cyan", "white",
        "blackBright", "redBright", "greenBright", "yellowBright",
        "blueBright", "magentaBright", "cyanBright", "whiteBright",
        "gray", "grey"
    };
    return names;
}

const std::vector<std::string>& backgroundColorNames() {
    static const std::vector<std::string> names = {
        "bgBlack", "bgRed", "bgGreen", "bgYellow", "bgBlue", "bgMagenta", "bgCyan", "bgWhite",
        "bgBlackBright", "bgRedBright", "bgGreenBright", "bgYellowBright",
        "bgBlueBright", "bgMagentaBright", "bgCyanBright", "bgWhiteBright",
        "bgGray", "bgGrey"
    };
    return names;
}

const std::vector<std::string>& colorNames() {
    static const std::vector<std::string> names = [] {
        std::vector<std::string> result;
        const auto& fg = foregroundColorNames();
        const auto& bg = backgroundColorNames();
        result.insert(result.end(), fg.begin(), fg.end());
        result.insert(result.end(), bg.begin(), bg.end());
        return result;
    }();
    return names;
}

} // namespace chalk
} // namespace polycpp
