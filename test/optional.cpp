#include "etl/optional.h"
#include "etl/string.h"
#include "etl/vector.h"
#include "etl/array.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project::etl;

TEST(Optional, Copy) {
    var a = optional<String<16>>("test %d", 123);
    EXPECT_TRUE(a);
    EXPECT_EQ(*a, "test 123");

    a = "test 1234";
    EXPECT_TRUE(a);
    EXPECT_EQ(*a, "test 1234");

    a = None;
    EXPECT_FALSE(a);
    EXPECT_EQ(a.get_value_or("None"), "None");
}

TEST(Optional, Move) {
    var v = vector(1, 2, 3);
    var a = optional<Vector<int>>(move(v));
    EXPECT_EQ(v.size(), 0);
    EXPECT_TRUE(a);
    EXPECT_EQ(*a, array(1,2,3));
}