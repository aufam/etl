#include "etl/numerics.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project::etl;

TEST(Numeric, Compare) {
    EXPECT_TRUE(safe_lt(-1, 10u));
    EXPECT_TRUE(safe_le(-1, 10.f));
    EXPECT_TRUE(safe_gt(2.0, 1));
    EXPECT_TRUE(safe_ge(char(100), -1));
    EXPECT_TRUE(safe_eq(char(100), 100));
    EXPECT_TRUE(safe_ne(char(100), -100));

    EXPECT_EQ(safe_add(-100, (unsigned char)(10)), -90);
    EXPECT_EQ(safe_sub(10u, 100u), -90);
    EXPECT_EQ(safe_mul(10u, -100), -1000);

    EXPECT_EQ(safe_mod(17.5, 4), 1.5);
    int a = safe_mod<int>(17.5, 4).get_value_or(0xffff);
    EXPECT_EQ(a, 1);
    EXPECT_EQ(safe_mod(17.5, 3.5), 0.0);
}