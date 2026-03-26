#include <gtest/gtest.h>
#include <polycpp/chalk/chalk.hpp>

using namespace polycpp::chalk;

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

TEST(ChalkTest, OperatorCallReturnsText) {
    Chalk c(Options{.level = 0});
    EXPECT_EQ(c("hello"), "hello");
}

TEST(ChalkTest, EmptyString) {
    Chalk c(Options{.level = 0});
    EXPECT_EQ(c(""), "");
}
