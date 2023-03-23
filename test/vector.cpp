#include "gtest/gtest.h"
#include "etl/vector.h"
#include "etl/keywords.h"

using namespace Project::etl;

TEST(Vector, append) {
    Vector<int> a = { 0, 1, 2 };
    Vector<int> b = { 3, 4, 5 };
    auto c = a + b;
    for (int i in range(6)) EXPECT_EQ(c[i], i);

    a += b;
    for (int i in range(6)) EXPECT_EQ(a[i], i);
}

TEST(Vector, remove) {
    Vector<int> a = { 0, 1, 2, 3, 4, 5, 5 };
    EXPECT_EQ(len(a), 7);
    EXPECT_EQ(a.back(), 5);
    a.remove(5);
    EXPECT_EQ(len(a), 6);
    for (int i in range(6)) EXPECT_EQ(a[i], i);
}

TEST(Vector, TypeDeduced) {
    auto a = vector(0, 1, 2, 3, 4, 5);
    EXPECT_EQ(len(a), 6);
    for (auto [x, y] in enumerate(a)) EXPECT_EQ(x, y);
}

TEST(Vector, Indexing) {
    auto a = vector(1, 2, 3);
    EXPECT_EQ(a[-1], 3); // last item
    EXPECT_EQ(a[-2], 2);
    EXPECT_EQ(a[-3], 1);
}

TEST(Vector, Swap) {
    auto a = vector(0, 1, 2);
    auto b = vector(3, 4, 5);
    swap_element(a, b); // swap each element
    EXPECT_EQ(b[0], 0);
    EXPECT_EQ(b[1], 1);
    EXPECT_EQ(b[2], 2);
    EXPECT_EQ(a[0], 3);
    EXPECT_EQ(a[1], 4);
    EXPECT_EQ(a[2], 5);
    swap(a, b); // swap vector
    EXPECT_EQ(a[0], 0);
    EXPECT_EQ(a[1], 1);
    EXPECT_EQ(a[2], 2);
    EXPECT_EQ(b[0], 3);
    EXPECT_EQ(b[1], 4);
    EXPECT_EQ(b[2], 5);
}

TEST(Vector, Move) {
    auto a = vector(0, 1, 2);
    auto b = move(a);
    EXPECT_EQ(b[0], 0);
    EXPECT_EQ(b[1], 1);
    EXPECT_EQ(b[2], 2);
}
