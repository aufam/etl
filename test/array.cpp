#include "gtest/gtest.h"
#include "etl/array.h"
#include "etl/bit.h"
#include "etl/string.h"
#include "etl/keywords.h"

using namespace Project::etl;
using namespace Project::etl::literals;

TEST(Array, Declaration) {
    val a = array(0, 1, 2);                 // using variadic function
    val b = array<int>(0.0, 1.0f, '\02');   // implicitly cast to the desired type
    val c = array<int, 3>({0, 1, 2});       // from initializer list
    val d = array<int, 3>();                // initialize to default value

    val x = {0, 1, 2};
    val y = {0, 0, 0};
    EXPECT_EQ(a, x);
    EXPECT_EQ(b, x);
    EXPECT_EQ(c, x);
    EXPECT_EQ(d, y);
}

TEST(Array, StructureBinding) {
    val [x, y, z] = array(1, 2, 3);
    EXPECT_EQ(x, 1);
    EXPECT_EQ(y, 2);
    EXPECT_EQ(z, 3);
}

TEST(Array, Indexing) {
    val constexpr a = array(1, 2, 3);
    EXPECT_EQ(a[-1], 3); // last item
    EXPECT_EQ(a[-2], 2);
    EXPECT_EQ(a[-3], 1);
    EXPECT_EQ(get<-1>(a), 3);
    EXPECT_EQ(get<-2>(a), 2);
    EXPECT_EQ(get<-3>(a), 1);
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

TEST(Array, Cast) {
    int a[] = {10, 1, 2, 3, 4, 5};

    // cast from a different kind of array
    var &b = array_cast(a);
    EXPECT_EQ(b, a);
    EXPECT_EQ(&b[0], &a[0]); // a and b share the same address
    EXPECT_EQ(len(b), len(a));
    b[0] = 0;
    EXPECT_EQ(b, range(6));

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
    EXPECT_TRUE(compare_all(b, range(1, 3)));

    val c = get<1, 3>(a); // creates new array
    EXPECT_EQ(c, range(1, 3));

    val &d = get<1, 3>(a); // creates reference to specific index
    EXPECT_EQ(d, range(1, 3));
}

TEST(Array, ByteConversion) {
    uint64_t const a = 0x1122334455667788ul;
    val b = byte_array_cast_le(a);
    EXPECT_EQ(b, array<uint8_t>(0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11));

    val c = byte_array_cast_be(a);
    EXPECT_EQ(c, array<uint8_t>(0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88));

    val d = byte_array_cast_back_le<uint64_t>(b);
    EXPECT_EQ(d, a);

    val e = byte_array_cast_back_be<uint64_t>(c);
    EXPECT_EQ(e, a);
}
