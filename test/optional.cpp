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
#if __cplusplus == 201703L
    EXPECT_EQ(*a, "test 123");
#endif

    a = "test 1234";
    EXPECT_TRUE(a);
#if __cplusplus == 201703L
    EXPECT_EQ(*a, "test 1234");
#endif

    a = none;
    EXPECT_FALSE(a);
#if __cplusplus == 201703L
    EXPECT_EQ(a.get_value_or("None"), "None");
#endif

    var b = optional(*a);
    EXPECT_FALSE(b);
}

TEST(Optional, Move) {
    var v = vector(1, 2, 3);
    var a = optional(move(v));
    EXPECT_EQ(v.size(), 0);
    EXPECT_TRUE(a);
#if __cplusplus == 201703L
    EXPECT_EQ(*a, array(1,2,3));
#endif
}

TEST(Optional, Empty) {
    float* p = nullptr;
    float& r = *p;
    var a = optional<int&>(r);

    EXPECT_FALSE(a);
}