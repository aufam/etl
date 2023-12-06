#include "etl/string_view.h"
#include "gtest/gtest.h"
#include <string_view>

using namespace Project::etl;
using namespace Project::etl::literals;

TEST(StringView, Split) {
    auto split = " 123  456 "sv.split(" ");
    for (auto &sv : split) {
        std::cout << "string view test: " << std::string_view(sv.data(), sv.len()) << "end\n";
    }
    EXPECT_EQ(split[0], "123");
    EXPECT_EQ(split[1], "456");
    EXPECT_EQ(split[2], "");
    EXPECT_EQ(split[3], "");
}

TEST(StringView, Match) {
    auto match = "12 34 "sv.match("%s %s");
    for (auto &sv : match) {
        std::cout << "string view test: " << std::string_view(sv.data(), sv.len()) << "end\n";
    }
    EXPECT_EQ(match[0], "12");
    EXPECT_EQ(match[1], "34 ");
    EXPECT_EQ(match[2], "");
}

TEST(StringView, MatchAPI) {
    auto match = "text/api/text/0"sv.match("%s/api/text/0");
    for (auto &sv : match) {
        std::cout << "string view test: " << std::string_view(sv.data(), sv.len()) << "end\n";
    }
    EXPECT_EQ(match[0], "text");
    EXPECT_EQ(match[1], "");
    EXPECT_EQ(match[2], "");
}
