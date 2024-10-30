#include "etl/optional.h"
#include "etl/string.h"
#include "etl/vector.h"
#include "etl/array.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project::etl;

TEST(Optional, Copy) {
    var a = optional(string<16>("test %d", 123));
    EXPECT_TRUE(a);
    EXPECT_EQ(*a, "test 123");

    a = "test 1234";
    EXPECT_TRUE(a);
    EXPECT_EQ(*a, "test 1234");

    a = none;
    EXPECT_FALSE(a);
    EXPECT_EQ(a.get_value_or("None"), "None");

    var b = optional(*a);
    EXPECT_FALSE(b);
}

TEST(Optional, Move) {
    var v = vector(1, 2, 3);
    var a = optional(move(v));
    EXPECT_EQ(v.size(), 0);
    EXPECT_TRUE(a);
    EXPECT_EQ(*a, array(1,2,3));
}

TEST(Optional, Empty) {
    float* p = nullptr;
    float& r = *p;
    var a = optional<int&>(r);

    EXPECT_FALSE(a);
}
