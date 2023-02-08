#include "gtest/gtest.h"
#include "etl/all.h"

using namespace Project::etl;

TEST(Vector, append) {
    Vector<int> a = { 0, 1, 2 };
    Vector<int> b = { 3, 4, 5 };
    auto c = a + b;
    for (int i = 0; i < 6; ++i) EXPECT_EQ(c[i], i);

    a += b;
    for (int i = 0; i < 6; ++i) EXPECT_EQ(a[i], i);
}