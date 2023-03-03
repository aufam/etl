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

TEST(Array, StructureBinding) {
    auto [x, y, z] = array(1, 2, 3);
    EXPECT_EQ(x, 1);
    EXPECT_EQ(y, 2);
    EXPECT_EQ(z, 3);
}

TEST(Array, Indexing) {
    constexpr auto a = array(1, 2, 3);
    EXPECT_EQ(a[-1], 3); // last item
    EXPECT_EQ(a[-2], 2);
    EXPECT_EQ(a[-3], 1);
    EXPECT_EQ(get<-1>(a), 3);
}

TEST(Array, Swap) {
    auto a = array(1, 2, 3);
    int b[] = {4, 5, 6};
    swap_element(a, b);
    EXPECT_EQ(b[0], 1);
    EXPECT_EQ(b[1], 2);
    EXPECT_EQ(b[2], 3);
    EXPECT_EQ(a[0], 4);
    EXPECT_EQ(a[1], 5);
    EXPECT_EQ(a[2], 6);
}