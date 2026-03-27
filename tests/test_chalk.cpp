#include <gtest/gtest.h>
#include <polycpp/chalk/detail/aggregator.hpp>
#include <polycpp/core/error.hpp>

using namespace polycpp::chalk;

// ═══ ANSI Styles Tests ══════════════════════════════

TEST(AnsiStylesTest, WrapAnsi16) {
    EXPECT_EQ(ansi::wrapAnsi16(31), "\x1B[31m");
    EXPECT_EQ(ansi::wrapAnsi16(0), "\x1B[0m");
    EXPECT_EQ(ansi::wrapAnsi16(97), "\x1B[97m");
}

TEST(AnsiStylesTest, WrapAnsi256Foreground) {
    EXPECT_EQ(ansi::wrapAnsi256(196), "\x1B[38;5;196m");
    EXPECT_EQ(ansi::wrapAnsi256(0), "\x1B[38;5;0m");
    EXPECT_EQ(ansi::wrapAnsi256(255), "\x1B[38;5;255m");
}

TEST(AnsiStylesTest, WrapAnsi256Background) {
    EXPECT_EQ(ansi::wrapAnsi256(196, true), "\x1B[48;5;196m");
    EXPECT_EQ(ansi::wrapAnsi256(0, true), "\x1B[48;5;0m");
}

TEST(AnsiStylesTest, WrapAnsi16mForeground) {
    EXPECT_EQ(ansi::wrapAnsi16m(255, 0, 0), "\x1B[38;2;255;0;0m");
    EXPECT_EQ(ansi::wrapAnsi16m(0, 255, 0), "\x1B[38;2;0;255;0m");
    EXPECT_EQ(ansi::wrapAnsi16m(0, 0, 255), "\x1B[38;2;0;0;255m");
}

TEST(AnsiStylesTest, WrapAnsi16mBackground) {
    EXPECT_EQ(ansi::wrapAnsi16m(255, 0, 0, true), "\x1B[48;2;255;0;0m");
    EXPECT_EQ(ansi::wrapAnsi16m(0, 128, 64, true), "\x1B[48;2;0;128;64m");
}

TEST(AnsiStylesTest, RgbToAnsi256Grayscale) {
    EXPECT_EQ(ansi::rgbToAnsi256(0, 0, 0), 16);       // black
    EXPECT_EQ(ansi::rgbToAnsi256(255, 255, 255), 231); // white
    EXPECT_EQ(ansi::rgbToAnsi256(128, 128, 128), 244); // mid gray
}

TEST(AnsiStylesTest, RgbToAnsi256Color) {
    EXPECT_EQ(ansi::rgbToAnsi256(255, 0, 0), 196);    // pure red
    EXPECT_EQ(ansi::rgbToAnsi256(0, 255, 0), 46);     // pure green
    EXPECT_EQ(ansi::rgbToAnsi256(0, 0, 255), 21);     // pure blue
}

TEST(AnsiStylesTest, HexToRgb6Char) {
    auto [r, g, b] = ansi::hexToRgb("#FF0000");
    EXPECT_EQ(r, 255); EXPECT_EQ(g, 0); EXPECT_EQ(b, 0);

    auto [r2, g2, b2] = ansi::hexToRgb("00FF00");
    EXPECT_EQ(r2, 0); EXPECT_EQ(g2, 255); EXPECT_EQ(b2, 0);

    auto [r3, g3, b3] = ansi::hexToRgb("#0000FF");
    EXPECT_EQ(r3, 0); EXPECT_EQ(g3, 0); EXPECT_EQ(b3, 255);
}

TEST(AnsiStylesTest, HexToRgb3Char) {
    auto [r, g, b] = ansi::hexToRgb("#F00");
    EXPECT_EQ(r, 255); EXPECT_EQ(g, 0); EXPECT_EQ(b, 0);

    auto [r2, g2, b2] = ansi::hexToRgb("0F0");
    EXPECT_EQ(r2, 0); EXPECT_EQ(g2, 255); EXPECT_EQ(b2, 0);

    auto [r3, g3, b3] = ansi::hexToRgb("#FFF");
    EXPECT_EQ(r3, 255); EXPECT_EQ(g3, 255); EXPECT_EQ(b3, 255);
}

TEST(AnsiStylesTest, HexToRgbInvalid) {
    auto [r, g, b] = ansi::hexToRgb("xyz");
    EXPECT_EQ(r, 0); EXPECT_EQ(g, 0); EXPECT_EQ(b, 0);

    auto [r2, g2, b2] = ansi::hexToRgb("");
    EXPECT_EQ(r2, 0); EXPECT_EQ(g2, 0); EXPECT_EQ(b2, 0);

    auto [r3, g3, b3] = ansi::hexToRgb("#GG");
    EXPECT_EQ(r3, 0); EXPECT_EQ(g3, 0); EXPECT_EQ(b3, 0);
}

TEST(AnsiStylesTest, HexToRgbLowercase) {
    auto [r, g, b] = ansi::hexToRgb("#ff8800");
    EXPECT_EQ(r, 255); EXPECT_EQ(g, 136); EXPECT_EQ(b, 0);
}

TEST(AnsiStylesTest, Ansi256ToAnsiBasic) {
    EXPECT_EQ(ansi::ansi256ToAnsi(0), 30);   // black
    EXPECT_EQ(ansi::ansi256ToAnsi(1), 31);   // red
    EXPECT_EQ(ansi::ansi256ToAnsi(2), 32);   // green
    EXPECT_EQ(ansi::ansi256ToAnsi(3), 33);   // yellow
    EXPECT_EQ(ansi::ansi256ToAnsi(4), 34);   // blue
    EXPECT_EQ(ansi::ansi256ToAnsi(5), 35);   // magenta
    EXPECT_EQ(ansi::ansi256ToAnsi(6), 36);   // cyan
    EXPECT_EQ(ansi::ansi256ToAnsi(7), 37);   // white
}

TEST(AnsiStylesTest, Ansi256ToAnsiBright) {
    EXPECT_EQ(ansi::ansi256ToAnsi(8), 90);   // bright black
    EXPECT_EQ(ansi::ansi256ToAnsi(9), 91);   // bright red
    EXPECT_EQ(ansi::ansi256ToAnsi(14), 96);  // bright cyan
    EXPECT_EQ(ansi::ansi256ToAnsi(15), 97);  // bright white
}

TEST(AnsiStylesTest, RgbToAnsi) {
    // Pure red -> ansi256=196 -> ansi=91
    EXPECT_EQ(ansi::rgbToAnsi(255, 0, 0), 91);
    // Pure green -> ansi256=46 -> ansi=92
    EXPECT_EQ(ansi::rgbToAnsi(0, 255, 0), 92);
    // Pure blue -> ansi256=21 -> ansi=94 (bright blue)
    EXPECT_EQ(ansi::rgbToAnsi(0, 0, 255), 94);
}

TEST(AnsiStylesTest, MakeColorCodeTruecolor) {
    EXPECT_EQ(ansi::makeColorCode(3, 255, 0, 0), "\x1B[38;2;255;0;0m");
    EXPECT_EQ(ansi::makeColorCode(3, 255, 0, 0, true), "\x1B[48;2;255;0;0m");
}

TEST(AnsiStylesTest, MakeColorCode256) {
    EXPECT_EQ(ansi::makeColorCode(2, 255, 0, 0), "\x1B[38;5;196m");
    EXPECT_EQ(ansi::makeColorCode(2, 255, 0, 0, true), "\x1B[48;5;196m");
}

TEST(AnsiStylesTest, MakeColorCode16) {
    // 255,0,0 -> rgbToAnsi=91 -> wrapAnsi16(91) for fg
    EXPECT_EQ(ansi::makeColorCode(1, 255, 0, 0), "\x1B[91m");
    // For bg: rgbToAnsi=91, +10=101 -> wrapAnsi16(101)
    EXPECT_EQ(ansi::makeColorCode(1, 255, 0, 0, true), "\x1B[101m");
}

TEST(AnsiStylesTest, MakeColorCodeHex) {
    EXPECT_EQ(ansi::makeColorCode(3, "#FF0000"), "\x1B[38;2;255;0;0m");
    EXPECT_EQ(ansi::makeColorCode(2, "#FF0000"), "\x1B[38;5;196m");
}

TEST(AnsiStylesTest, MakeColorCode256Index) {
    EXPECT_EQ(ansi::makeColorCode256(2, 196), "\x1B[38;5;196m");
    EXPECT_EQ(ansi::makeColorCode256(2, 196, true), "\x1B[48;5;196m");
    EXPECT_EQ(ansi::makeColorCode256(3, 196), "\x1B[38;5;196m");
}

TEST(AnsiStylesTest, MakeColorCode256AlwaysOutputs256) {
    // JS chalk's ansi256() always outputs 256-color format, never downsamples.
    // getModelAnsi falls through to ansiStyles[type][model]() regardless of level.
    EXPECT_EQ(ansi::makeColorCode256(1, 196), "\x1B[38;5;196m");
    EXPECT_EQ(ansi::makeColorCode256(1, 196, true), "\x1B[48;5;196m");
}

// ═══ Chalk Core Tests ═══════════════════════════════

TEST(ChalkTest, DefaultConstruction) {
    Chalk c;
    EXPECT_EQ(c.level(), 0);
}

TEST(ChalkTest, OptionsLevel) {
    Chalk c(Options{.level = 2});
    EXPECT_EQ(c.level(), 2);
}

TEST(ChalkTest, SetLevel) {
    Chalk c;
    c.setLevel(3);
    EXPECT_EQ(c.level(), 3);
}

TEST(ChalkTest, InvalidLevelThrows) {
    // JS chalk: "The `level` option should be an integer from 0 to 3"
    EXPECT_THROW(Chalk(Options{.level = 5}), polycpp::Error);
    EXPECT_THROW(Chalk(Options{.level = -1}), polycpp::Error);
    EXPECT_NO_THROW(Chalk(Options{.level = 0}));
    EXPECT_NO_THROW(Chalk(Options{.level = 3}));
}

TEST(ChalkTest, Level0ReturnsPlainText) {
    Chalk c(Options{.level = 0});
    EXPECT_EQ(c.red()("hello"), "hello");
    EXPECT_EQ(c.bold().red()("hello"), "hello");
}

TEST(ChalkTest, EmptyStringReturnsEmpty) {
    Chalk c(Options{.level = 3});
    EXPECT_EQ(c.red()(""), "");
}

TEST(ChalkTest, NoStyleReturnsPlainText) {
    Chalk c(Options{.level = 3});
    EXPECT_EQ(c("hello"), "hello");
}

TEST(ChalkTest, BasicRed) {
    Chalk c(Options{.level = 1});
    EXPECT_EQ(c.red()("hello"), "\x1B[31mhello\x1B[39m");
}

TEST(ChalkTest, BasicBold) {
    Chalk c(Options{.level = 1});
    EXPECT_EQ(c.bold()("hello"), "\x1B[1mhello\x1B[22m");
}

TEST(ChalkTest, ChainRedBold) {
    Chalk c(Options{.level = 1});
    EXPECT_EQ(c.red().bold()("hello"), "\x1B[31m\x1B[1mhello\x1B[22m\x1B[39m");
}

TEST(ChalkTest, ChainBoldRed) {
    Chalk c(Options{.level = 1});
    EXPECT_EQ(c.bold().red()("hello"), "\x1B[1m\x1B[31mhello\x1B[39m\x1B[22m");
}

TEST(ChalkTest, Nesting) {
    Chalk c(Options{.level = 1});
    std::string inner = c.blue()("b");
    std::string result = c.red()("a" + inner + "c");
    // Inner: \x1B[34mb\x1B[39m
    // Outer red wraps the whole thing, replacing inner's \x1B[39m with \x1B[39m\x1B[31m
    EXPECT_EQ(result, "\x1B[31ma\x1B[34mb\x1B[39m\x1B[31mc\x1B[39m");
}

TEST(ChalkTest, NestingMultipleLevels) {
    Chalk c(Options{.level = 1});
    std::string innermost = c.green()("g");
    std::string middle = c.blue()("b" + innermost + "b");
    std::string result = c.red()("r" + middle + "r");
    // Innermost: \x1B[32mg\x1B[39m
    // Middle wraps it: blue around "b" + innermost + "b"
    //   blue replaces \x1B[39m with \x1B[39m\x1B[34m in innermost
    //   => \x1B[34mb\x1B[32mg\x1B[39m\x1B[34mb\x1B[39m
    // Outer red wraps middle: replaces \x1B[39m with \x1B[39m\x1B[31m
    //   => \x1B[31mr\x1B[34mb\x1B[32mg\x1B[39m\x1B[31m\x1B[34mb\x1B[39m\x1B[31mr\x1B[39m
    // Wait -- the outer red close (\x1B[39m) also gets replaced at ALL occurrences in the text
    // Actually we need to be more careful: the replaceCloseWithReopen handles the close codes for the red styler
    // Let me trace through carefully:
    // middle = "\x1B[34mb\x1B[32mg\x1B[39m\x1B[34mb\x1B[39m"
    // red wraps "r" + middle + "r" = "r\x1B[34mb\x1B[32mg\x1B[39m\x1B[34mb\x1B[39mr"
    // The red styler has close = "\x1B[39m"
    // replaceCloseWithReopen replaces "\x1B[39m" with "\x1B[39m\x1B[31m"
    // Result after replace: "r\x1B[34mb\x1B[32mg\x1B[39m\x1B[31m\x1B[34mb\x1B[39m\x1B[31mr"
    // Then wrap with openAll + ... + closeAll:
    // "\x1B[31m" + "r\x1B[34mb\x1B[32mg\x1B[39m\x1B[31m\x1B[34mb\x1B[39m\x1B[31mr" + "\x1B[39m"
    EXPECT_EQ(result, "\x1B[31mr\x1B[34mb\x1B[32mg\x1B[39m\x1B[31m\x1B[34mb\x1B[39m\x1B[31mr\x1B[39m");
}

TEST(ChalkTest, LineBreak) {
    Chalk c(Options{.level = 1});
    std::string result = c.red()("hello\nworld");
    EXPECT_EQ(result, "\x1B[31mhello\x1B[39m\n\x1B[31mworld\x1B[39m");
}

TEST(ChalkTest, CRLF) {
    Chalk c(Options{.level = 1});
    std::string result = c.red()("hello\r\nworld");
    EXPECT_EQ(result, "\x1B[31mhello\x1B[39m\r\n\x1B[31mworld\x1B[39m");
}

TEST(ChalkTest, MultipleLineBreaks) {
    Chalk c(Options{.level = 1});
    std::string result = c.red()("a\nb\nc");
    EXPECT_EQ(result, "\x1B[31ma\x1B[39m\n\x1B[31mb\x1B[39m\n\x1B[31mc\x1B[39m");
}

TEST(ChalkTest, BackgroundColor) {
    Chalk c(Options{.level = 1});
    EXPECT_EQ(c.bgRed()("hello"), "\x1B[41mhello\x1B[49m");
}

TEST(ChalkTest, ForegroundAndBackground) {
    Chalk c(Options{.level = 1});
    EXPECT_EQ(c.red().bgBlue()("hello"),
              "\x1B[31m\x1B[44mhello\x1B[49m\x1B[39m");
}

TEST(ChalkTest, Rgb) {
    Chalk c(Options{.level = 3});
    EXPECT_EQ(c.rgb(255, 0, 0)("hello"), "\x1B[38;2;255;0;0mhello\x1B[39m");
}

TEST(ChalkTest, Hex) {
    Chalk c(Options{.level = 3});
    EXPECT_EQ(c.hex("#FF0000")("hello"), "\x1B[38;2;255;0;0mhello\x1B[39m");
}

TEST(ChalkTest, HexWithoutHash) {
    Chalk c(Options{.level = 3});
    EXPECT_EQ(c.hex("00FF00")("hello"), "\x1B[38;2;0;255;0mhello\x1B[39m");
}

TEST(ChalkTest, HexShorthand) {
    Chalk c(Options{.level = 3});
    EXPECT_EQ(c.hex("#F00")("hello"), "\x1B[38;2;255;0;0mhello\x1B[39m");
}

TEST(ChalkTest, Ansi256Color) {
    Chalk c(Options{.level = 2});
    EXPECT_EQ(c.ansi256(196)("hello"), "\x1B[38;5;196mhello\x1B[39m");
}

TEST(ChalkTest, BgRgb) {
    Chalk c(Options{.level = 3});
    EXPECT_EQ(c.bgRgb(255, 0, 0)("hello"), "\x1B[48;2;255;0;0mhello\x1B[49m");
}

TEST(ChalkTest, BgHex) {
    Chalk c(Options{.level = 3});
    EXPECT_EQ(c.bgHex("#FF0000")("hello"), "\x1B[48;2;255;0;0mhello\x1B[49m");
}

TEST(ChalkTest, BgAnsi256) {
    Chalk c(Options{.level = 2});
    EXPECT_EQ(c.bgAnsi256(196)("hello"), "\x1B[48;5;196mhello\x1B[49m");
}

TEST(ChalkTest, VisibleWithColor) {
    Chalk c(Options{.level = 1});
    EXPECT_EQ(c.visible()("hello"), "hello");  // no style, just text
}

TEST(ChalkTest, VisibleWithoutColor) {
    Chalk c(Options{.level = 0});
    EXPECT_EQ(c.visible()("hello"), "");  // empty when level is 0
}

TEST(ChalkTest, VisibleWithStyle) {
    Chalk c(Options{.level = 1});
    // visible() with red: text should still be styled
    EXPECT_EQ(c.red().visible()("hello"), "\x1B[31mhello\x1B[39m");
}

TEST(ChalkTest, VisibleEmptyAtLevel0) {
    Chalk c(Options{.level = 0});
    EXPECT_EQ(c.red().visible()("hello"), "");
}

TEST(ChalkTest, MultipleArgs) {
    Chalk c(Options{.level = 1});
    EXPECT_EQ(c.red()("hello", "world"), "\x1B[31mhello world\x1B[39m");
}

TEST(ChalkTest, MultipleArgsWithNumbers) {
    Chalk c(Options{.level = 1});
    EXPECT_EQ(c.red()("count:", 42), "\x1B[31mcount: 42\x1B[39m");
}

TEST(ChalkTest, ReusableTheme) {
    Chalk c(Options{.level = 1});
    auto error = c.bold().red();
    EXPECT_EQ(error("fail"), "\x1B[1m\x1B[31mfail\x1B[39m\x1B[22m");
    EXPECT_EQ(error("again"), "\x1B[1m\x1B[31magain\x1B[39m\x1B[22m");
}

TEST(ChalkTest, RgbDownsampleTo256) {
    Chalk c(Options{.level = 2});
    std::string result = c.rgb(255, 0, 0)("hello");
    EXPECT_EQ(result, "\x1B[38;5;196mhello\x1B[39m");
}

TEST(ChalkTest, RgbDownsampleTo16) {
    Chalk c(Options{.level = 1});
    std::string result = c.rgb(255, 0, 0)("hello");
    // 255,0,0 -> ansi256=196 -> ansi=91 -> wrapAnsi16(91) = "\x1B[91m"
    EXPECT_EQ(result, "\x1B[91mhello\x1B[39m");
}

TEST(ChalkTest, BgRgbDownsampleTo16) {
    Chalk c(Options{.level = 1});
    std::string result = c.bgRgb(255, 0, 0)("hello");
    // 255,0,0 -> rgbToAnsi=91, +10=101 -> wrapAnsi16(101) = "\x1B[101m"
    EXPECT_EQ(result, "\x1B[101mhello\x1B[49m");
}

TEST(ChalkTest, AllModifiers) {
    Chalk c(Options{.level = 1});
    EXPECT_EQ(c.reset()("x"), "\x1B[0mx\x1B[0m");
    EXPECT_EQ(c.bold()("x"), "\x1B[1mx\x1B[22m");
    EXPECT_EQ(c.dim()("x"), "\x1B[2mx\x1B[22m");
    EXPECT_EQ(c.italic()("x"), "\x1B[3mx\x1B[23m");
    EXPECT_EQ(c.underline()("x"), "\x1B[4mx\x1B[24m");
    EXPECT_EQ(c.overline()("x"), "\x1B[53mx\x1B[55m");
    EXPECT_EQ(c.inverse()("x"), "\x1B[7mx\x1B[27m");
    EXPECT_EQ(c.hidden()("x"), "\x1B[8mx\x1B[28m");
    EXPECT_EQ(c.strikethrough()("x"), "\x1B[9mx\x1B[29m");
}

TEST(ChalkTest, AllForegroundColors) {
    Chalk c(Options{.level = 1});
    EXPECT_EQ(c.black()("x"), "\x1B[30mx\x1B[39m");
    EXPECT_EQ(c.red()("x"), "\x1B[31mx\x1B[39m");
    EXPECT_EQ(c.green()("x"), "\x1B[32mx\x1B[39m");
    EXPECT_EQ(c.yellow()("x"), "\x1B[33mx\x1B[39m");
    EXPECT_EQ(c.blue()("x"), "\x1B[34mx\x1B[39m");
    EXPECT_EQ(c.magenta()("x"), "\x1B[35mx\x1B[39m");
    EXPECT_EQ(c.cyan()("x"), "\x1B[36mx\x1B[39m");
    EXPECT_EQ(c.white()("x"), "\x1B[37mx\x1B[39m");
    EXPECT_EQ(c.blackBright()("x"), "\x1B[90mx\x1B[39m");
    EXPECT_EQ(c.redBright()("x"), "\x1B[91mx\x1B[39m");
    EXPECT_EQ(c.greenBright()("x"), "\x1B[92mx\x1B[39m");
    EXPECT_EQ(c.yellowBright()("x"), "\x1B[93mx\x1B[39m");
    EXPECT_EQ(c.blueBright()("x"), "\x1B[94mx\x1B[39m");
    EXPECT_EQ(c.magentaBright()("x"), "\x1B[95mx\x1B[39m");
    EXPECT_EQ(c.cyanBright()("x"), "\x1B[96mx\x1B[39m");
    EXPECT_EQ(c.whiteBright()("x"), "\x1B[97mx\x1B[39m");
}

TEST(ChalkTest, AllBackgroundColors) {
    Chalk c(Options{.level = 1});
    EXPECT_EQ(c.bgBlack()("x"), "\x1B[40mx\x1B[49m");
    EXPECT_EQ(c.bgRed()("x"), "\x1B[41mx\x1B[49m");
    EXPECT_EQ(c.bgGreen()("x"), "\x1B[42mx\x1B[49m");
    EXPECT_EQ(c.bgYellow()("x"), "\x1B[43mx\x1B[49m");
    EXPECT_EQ(c.bgBlue()("x"), "\x1B[44mx\x1B[49m");
    EXPECT_EQ(c.bgMagenta()("x"), "\x1B[45mx\x1B[49m");
    EXPECT_EQ(c.bgCyan()("x"), "\x1B[46mx\x1B[49m");
    EXPECT_EQ(c.bgWhite()("x"), "\x1B[47mx\x1B[49m");
    EXPECT_EQ(c.bgBlackBright()("x"), "\x1B[100mx\x1B[49m");
    EXPECT_EQ(c.bgRedBright()("x"), "\x1B[101mx\x1B[49m");
    EXPECT_EQ(c.bgGreenBright()("x"), "\x1B[102mx\x1B[49m");
    EXPECT_EQ(c.bgYellowBright()("x"), "\x1B[103mx\x1B[49m");
    EXPECT_EQ(c.bgBlueBright()("x"), "\x1B[104mx\x1B[49m");
    EXPECT_EQ(c.bgMagentaBright()("x"), "\x1B[105mx\x1B[49m");
    EXPECT_EQ(c.bgCyanBright()("x"), "\x1B[106mx\x1B[49m");
    EXPECT_EQ(c.bgWhiteBright()("x"), "\x1B[107mx\x1B[49m");
}

TEST(ChalkTest, GrayAliases) {
    Chalk c(Options{.level = 1});
    EXPECT_EQ(c.gray()("x"), c.blackBright()("x"));
    EXPECT_EQ(c.grey()("x"), c.blackBright()("x"));
    EXPECT_EQ(c.bgGray()("x"), c.bgBlackBright()("x"));
    EXPECT_EQ(c.bgGrey()("x"), c.bgBlackBright()("x"));
}

TEST(ChalkTest, LineBreakWithChainedStyles) {
    Chalk c(Options{.level = 1});
    std::string result = c.bold().red()("hello\nworld");
    // With bold+red chained: openAll = "\x1B[1m\x1B[31m", closeAll = "\x1B[39m\x1B[22m"
    EXPECT_EQ(result,
        "\x1B[1m\x1B[31mhello\x1B[39m\x1B[22m\n\x1B[1m\x1B[31mworld\x1B[39m\x1B[22m");
}

TEST(ChalkTest, NestingWithBackgroundColors) {
    Chalk c(Options{.level = 1});
    std::string inner = c.bgGreen()("g");
    std::string result = c.bgRed()("r" + inner + "r");
    // Inner: \x1B[42mg\x1B[49m
    // Outer bgRed replaces \x1B[49m with \x1B[49m\x1B[41m
    EXPECT_EQ(result, "\x1B[41mr\x1B[42mg\x1B[49m\x1B[41mr\x1B[49m");
}

// ═══ Tests Ported from JS chalk test/chalk.js ═══════

// JS test: "support nesting styles" (line 38-43)
TEST(ChalkTest, JsNestingWithMixedStyles) {
    Chalk c(Options{.level = 3});
    std::string result = c.red()("foo" + c.underline().bgBlue()("bar") + "!");
    EXPECT_EQ(result, "\x1B[31mfoo\x1B[4m\x1B[44mbar\x1B[49m\x1B[24m!\x1B[39m");
}

// JS test: "support nesting styles of the same type" (line 45-50)
TEST(ChalkTest, JsNestingSameType) {
    Chalk c(Options{.level = 3});
    auto inner = c.green()("c");
    auto middle = c.yellow()("b" + inner + "b");
    auto result = c.red()("a" + middle + "c");
    EXPECT_EQ(result,
        "\x1B[31ma\x1B[33mb\x1B[32mc\x1B[39m\x1B[31m\x1B[33mb\x1B[39m\x1B[31mc\x1B[39m");
}

// JS test: "support applying multiple styles at once" (line 33-36)
TEST(ChalkTest, JsMultipleStyles) {
    Chalk c(Options{.level = 3});
    EXPECT_EQ(c.red().bgGreen().underline()("foo"),
        "\x1B[31m\x1B[42m\x1B[4mfoo\x1B[24m\x1B[49m\x1B[39m");
    EXPECT_EQ(c.underline().red().bgGreen()("foo"),
        "\x1B[4m\x1B[31m\x1B[42mfoo\x1B[49m\x1B[39m\x1B[24m");
}

// JS test: "properly convert RGB to 16 colors on basic color terminals" (line 97-100)
TEST(ChalkTest, JsRgbTo16) {
    Chalk c(Options{.level = 1});
    EXPECT_EQ(c.hex("#FF0000")("hello"), "\x1B[91mhello\x1B[39m");
    EXPECT_EQ(c.bgHex("#FF0000")("hello"), "\x1B[101mhello\x1B[49m");
}

// JS test: "properly convert RGB to 256 colors" (line 102-106)
TEST(ChalkTest, JsRgbTo256And16m) {
    EXPECT_EQ(Chalk(Options{.level = 2}).hex("#FF0000")("hello"),
        "\x1B[38;5;196mhello\x1B[39m");
    EXPECT_EQ(Chalk(Options{.level = 2}).bgHex("#FF0000")("hello"),
        "\x1B[48;5;196mhello\x1B[49m");
    EXPECT_EQ(Chalk(Options{.level = 3}).bgHex("#FF0000")("hello"),
        "\x1B[48;2;255;0;0mhello\x1B[49m");
}

// JS test: "don't emit RGB codes if level is 0" (line 108-111)
TEST(ChalkTest, JsNoCodesAtLevel0) {
    EXPECT_EQ(Chalk(Options{.level = 0}).hex("#FF0000")("hello"), "hello");
    EXPECT_EQ(Chalk(Options{.level = 0}).bgHex("#FF0000")("hello"), "hello");
}

// JS test: "don't output escape codes if the input is empty" (line 78-81)
TEST(ChalkTest, JsEmptyInput) {
    Chalk c(Options{.level = 3});
    EXPECT_EQ(c.red()(""), "");
    EXPECT_EQ(c.red().blue().black()(""), "");
}

// JS test: "support falsy values" (line 74-76)
TEST(ChalkTest, JsFalsyValues) {
    Chalk c(Options{.level = 3});
    EXPECT_EQ(c.red()(0), "\x1B[31m0\x1B[39m");
}

// JS test: "reset all styles with .reset()" (line 52-54)
TEST(ChalkTest, JsResetWrapping) {
    Chalk c(Options{.level = 3});
    std::string styled = c.red().bgGreen().underline()("foo");
    std::string result = c.reset()(styled + "foo");
    EXPECT_EQ(result,
        "\x1B[0m\x1B[31m\x1B[42m\x1B[4mfoo\x1B[24m\x1B[49m\x1B[39mfoo\x1B[0m");
}

// Bug fix test: ansi256 should NOT downsample at level 1
// JS chalk always outputs 256-color format for ansi256() regardless of level
TEST(ChalkTest, Ansi256NoDownsampleAtLevel1) {
    Chalk c(Options{.level = 1});
    EXPECT_EQ(c.ansi256(196)("hello"), "\x1B[38;5;196mhello\x1B[39m");
    EXPECT_EQ(c.bgAnsi256(196)("hello"), "\x1B[48;5;196mhello\x1B[49m");
}

// ═══ Color Support Detection Tests ══════════════════

TEST(SupportsColorTest, ReturnsValidLevel) {
    auto cs = supportsColor();
    EXPECT_GE(cs.level, 0);
    EXPECT_LE(cs.level, 3);
    EXPECT_EQ(cs.hasBasic, cs.level >= 1);
    EXPECT_EQ(cs.has256, cs.level >= 2);
    EXPECT_EQ(cs.has16m, cs.level >= 3);
}

TEST(SupportsColorTest, StderrReturnsValidLevel) {
    auto cs = supportsColorStderr();
    EXPECT_GE(cs.level, 0);
    EXPECT_LE(cs.level, 3);
    EXPECT_EQ(cs.hasBasic, cs.level >= 1);
    EXPECT_EQ(cs.has256, cs.level >= 2);
    EXPECT_EQ(cs.has16m, cs.level >= 3);
}

// ═══ Name Array Tests ═══════════════════════════════

TEST(NameArraysTest, ModifierNames) {
    auto& names = modifierNames();
    EXPECT_EQ(names.size(), 9u);
    EXPECT_EQ(names[0], "reset");
    EXPECT_EQ(names[1], "bold");
    EXPECT_EQ(names[2], "dim");
    EXPECT_EQ(names[3], "italic");
    EXPECT_EQ(names[4], "underline");
    EXPECT_EQ(names[5], "overline");
    EXPECT_EQ(names[6], "inverse");
    EXPECT_EQ(names[7], "hidden");
    EXPECT_EQ(names[8], "strikethrough");
}

TEST(NameArraysTest, ForegroundColorNames) {
    auto& names = foregroundColorNames();
    EXPECT_EQ(names.size(), 18u);  // 16 + gray + grey
    EXPECT_EQ(names[0], "black");
    EXPECT_EQ(names[1], "red");
    EXPECT_EQ(names[16], "gray");
    EXPECT_EQ(names[17], "grey");
}

TEST(NameArraysTest, BackgroundColorNames) {
    auto& names = backgroundColorNames();
    EXPECT_EQ(names.size(), 18u);  // 16 + bgGray + bgGrey
    EXPECT_EQ(names[0], "bgBlack");
    EXPECT_EQ(names[1], "bgRed");
    EXPECT_EQ(names[16], "bgGray");
    EXPECT_EQ(names[17], "bgGrey");
}

TEST(NameArraysTest, ColorNamesUnion) {
    auto& names = colorNames();
    EXPECT_EQ(names.size(), 36u);  // 18 fg + 18 bg
}

// ═══ Global Instance Tests ══════════════════════════

TEST(GlobalInstanceTest, ChalkInstanceExists) {
    auto& c = chalk();
    EXPECT_GE(c.level(), 0);
    EXPECT_LE(c.level(), 3);
}

TEST(GlobalInstanceTest, ChalkStderrExists) {
    auto& c = chalkStderr();
    EXPECT_GE(c.level(), 0);
    EXPECT_LE(c.level(), 3);
}

TEST(GlobalInstanceTest, ChalkInstanceIsMutable) {
    auto& c = chalk();
    int original = c.level();
    c.setLevel(1);
    EXPECT_EQ(c.level(), 1);
    c.setLevel(original);  // restore
}
