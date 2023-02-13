#include "gtest/gtest.h"
#include "etl/vector.h"
#include "etl/utility.h"
#include "etl/python_keywords.h"

using namespace Project::etl;

TEST(Vector, append) {
    Vector<int> a = { 0, 1, 2 };
    Vector<int> b = { 3, 4, 5 };
    auto c = a + b;
    for (int i in range(6)) EXPECT_EQ(c[i], i);

    a += b;
    for (int i in range(6)) EXPECT_EQ(a[i], i);
}