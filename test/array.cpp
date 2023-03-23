#include "gtest/gtest.h"
#include "etl/array.h"
#include "etl/bit.h"
#include "etl/keywords.h"

using namespace Project::etl;

TEST(Array, Empty) {
    val a = array<int, 5>();
    foreach(a, lambda (auto& item) { EXPECT_EQ(item, 0); });
}

TEST(Array, TypeAndSizeDeduced) {
    val a = array(0, 1, 2, 3, 4, 5, 6, 7, 8);
    for (auto [x, y] in enumerate(a)) EXPECT_EQ(x, y);
}

TEST(Array, StructureBinding) {
    val [x, y, z] = array(1, 2, 3);
    EXPECT_EQ(x, 1);
    EXPECT_EQ(y, 2);
    EXPECT_EQ(z, 3);
}

TEST(Array, Indexing) {
    constexpr val a = array(1, 2, 3);
    EXPECT_EQ(a[-1], 3); // last item
    EXPECT_EQ(a[-2], 2);
    EXPECT_EQ(a[-3], 1);
    EXPECT_EQ(get<-1>(a), 3);
}

TEST(Array, Swap) {
    var a = array(1, 2, 3);
    int b[] = {4, 5, 6};
    swap(a, b);
    EXPECT_EQ(b[0], 1);
    EXPECT_EQ(b[1], 2);
    EXPECT_EQ(b[2], 3);
    EXPECT_EQ(a[0], 4);
    EXPECT_EQ(a[1], 5);
    EXPECT_EQ(a[2], 6);
}

TEST(Array, Move) {
    var a = array(1, 2, 3);
    var b = move(a);
    EXPECT_EQ(b[0], 1);
    EXPECT_EQ(b[1], 2);
    EXPECT_EQ(b[2], 3);
}

TEST(Array, Cast) {
    int a[] = {10, 1, 2, 3, 4, 5};

    // cast from different kind of array
    var& b = array_cast(a);
    EXPECT_TRUE(compare_all(b, a));
    EXPECT_EQ(&b[0], &a[0]); // a and b share the same address
    EXPECT_EQ(len(b), len(a));
    b[0] = 0;
    EXPECT_EQ(b, array(0, 1, 2, 3, 4, 5));

    // cast to other type
    val& c = array_cast<float>(b);
    EXPECT_EQ((size_t)&a[0], (size_t)&c[0]);
    EXPECT_EQ(len(a), len(c));
    for (val [x, y] in zip(a, c)) EXPECT_EQ(x, bit_cast<int>(y));

    // cast from pointer
    val& d = array_cast<int, 5>(&a[1]);
    EXPECT_EQ(&a[1], &d[0]);
    EXPECT_EQ(d, array(1, 2, 3, 4, 5));
    EXPECT_EQ(len(d), 5);

    val e = triple(1, 0.1, 2);
    val& f = array_cast<int>(e);
    EXPECT_EQ((size_t)&e, (size_t)&f);
    EXPECT_EQ(len(f), sizeof(e) / sizeof(int));
}

TEST(Array, Slice) {
    val a = array(0, 1, 2, 3, 4);
    val b = a(1, 3);
    EXPECT_EQ(b[0], 1);
    EXPECT_EQ(b[1], 2);
}
