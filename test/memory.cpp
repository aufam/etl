#include "etl/memory.h"
#include "etl/string.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project::etl;

TEST(Memory, Unique) {
    var a = make_unique<String<16>>("test %d", 123);
    var b = move(a);
    EXPECT_FALSE(a);
    EXPECT_EQ(*b, "test 123");
}

TEST(Memory, Shared) {
    var a = make_shared<String<16>>("test %d", 123);
    var b = a;
    var c = move(a);
    EXPECT_FALSE(a);
    EXPECT_EQ(*c, "test 123");
    EXPECT_EQ(*b, "test 123");
}