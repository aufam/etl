#include "gtest/gtest.h"
#include "etl/array.h"
#include "etl/algorithm.h"
#include "etl/python_keywords.h"

using namespace Project::etl;

TEST(Array, Empty) {
    Array<int, 5> a = {};
    foreach(a, [](auto& item) { EXPECT_EQ(item, 0); });
}

TEST(Array, TypeAndSizeDeduced) {
    auto a = array(0, 1, 2, 3, 4, 5, 6, 7, 8);
    for (auto [x, y] in enumerate(a)) EXPECT_EQ(x, y);
}