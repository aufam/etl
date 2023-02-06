#include "gtest/gtest.h"
#include "etl/string.h"

bool IsEq(const char* a, const char* b) { return strcmp(a, b) == 0; }

TEST(String, Push) {
    Project::etl::String s;
    s("Test");
    s += '\n';
    EXPECT_PRED2(IsEq, "Test\n", s.data());
}

TEST(String, Append) {
    Project::etl::String s;
    s("Test");
    s += "123";
    EXPECT_PRED2(IsEq, "Test123", s.data());
}

TEST(String, Assign) {
    Project::etl::String s { "Test "};
    s = "Test123";
    EXPECT_PRED2(IsEq, "Test123", s.data());
}

TEST(String, IsContaining) {
    const Project::etl::String s {"Test %d%d%d", 1, 2, 3};
    EXPECT_TRUE(s.isContaining("123"));
    EXPECT_TRUE(!s.isContaining("321"));
}