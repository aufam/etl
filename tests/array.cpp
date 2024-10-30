#include "gtest/gtest.h"
#include "etl/array.h"
#include "etl/bit.h"
#include "etl/keywords.h"

using namespace Project::etl;

TEST(Array, Declaration) {
    val a = array(0, 1, 2);                 // using variadic function
    val b = array<int>(0.0, 1.0f, '\02');   // implicitly cast to the desired type
    val c = array<int, 3>({0, 1, 2});       // from initializer list
    val d = array<int, 3>();                // initialize to default value

    val x = array(0, 1, 2);
    val y = array(0, 0, 0);
    EXPECT_EQ(a, x);
    EXPECT_EQ(b, x);
    EXPECT_EQ(c, x);
    EXPECT_EQ(d, y);
}

TEST(Array, Empty) {
    val a = array<int, 0>();
    EXPECT_FALSE(a);
    EXPECT_EQ(a, none);
}

TEST(Array, Indexing) {
    val constexpr a = array(1, 2, 3);
    EXPECT_EQ(a[-1], 3); // last item
    EXPECT_EQ(a[-2], 2);
    EXPECT_EQ(a[-3], 1);
}

TEST(Array, Swap) {
    var a = array(1, 2, 3);
    int b[] = {4, 5, 6};
    swap_element(a, b);
    EXPECT_EQ(b[0], 1);
    EXPECT_EQ(b[1], 2);
    EXPECT_EQ(b[2], 3);
    EXPECT_EQ(a[0], 4);
    EXPECT_EQ(a[1], 5);
    EXPECT_EQ(a[2], 6);
}

TEST(Array, Cast) {
    int a[] = {10, 1, 2, 3, 4, 5};

    // cast from a different kind of array
    var &b = array_cast(a);
    EXPECT_EQ(&b[0], &a[0]); // a and b share the same address
    EXPECT_EQ(len(b), len(a));
    b[0] = 0;
    EXPECT_EQ(b, array(0, 1, 2, 3, 4, 5));

    // cast to another type
    val &c = array_cast<float>(b);
    EXPECT_EQ((size_t)&a[0], (size_t)&c[0]);
    EXPECT_EQ(len(a), len(c));
    for (var [x, y] in zip(a, c)) EXPECT_EQ(x, bit_cast<int>(y));

    // cast from a pointer
    val &d = array_cast<int, 5>(&a[1]);
    EXPECT_EQ(&a[1], &d[0]);
    EXPECT_EQ(d, array(1, 2, 3, 4, 5));
    EXPECT_EQ(len(d), 5);

    // cast from another type
    val e = triple(1, 0.1, 2);
    val &f = array_cast<int>(e);
    EXPECT_EQ((size_t)&e, (size_t)&f);
    EXPECT_EQ(len(f), sizeof(e) / sizeof(int));
}

TEST(Array, Slice) {
    val a = array(0, 1, 2, 3, 4);

    val b = a(1, 3); // creates iter object
    EXPECT_EQ(b, array(1, 2));
}

