#include "etl/vector.h"
#include "etl/string.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project::etl;

TEST(Vector, Declaration) {
    val a = vector(0, 1, 2);                // using variadic function
    val b = vector<int>(0.0, 1.0f, '\02');  // implicitly cast to the desired type
    val c = vector<int>({0, 1, 2});         // from initializer list
    val d = vector<int>();                  // empty vector

    val x = {0, 1, 2};
    EXPECT_EQ(a, x);
    EXPECT_EQ(b, x);
    EXPECT_EQ(c, x);
    EXPECT_NE(d, x);
}

TEST(Vector, Append) {
    var a = vector(0, 1, 2);
    val b = vector(3, 4, 5);

    val c = a + b;
    EXPECT_EQ(c, range(6));

    a += b;
    EXPECT_EQ(a, range(6));
}

TEST(Vector, Remove) {
    var a = vector(0, 1, 2, 3, 4, 5, 5);
    EXPECT_EQ(len(a), 7);
    EXPECT_EQ(a.back(), 5);

    a.remove(5);
    EXPECT_EQ(len(a), 6);
    EXPECT_EQ(a, range(6));

    a.remove_at(-1); // remove last item
    EXPECT_EQ(len(a), 5);
    EXPECT_EQ(a, range(5));
}

TEST(Vector, Indexing) {
    val a = vector(1, 2, 3);
    EXPECT_EQ(a[-1], 3); // last item
    EXPECT_EQ(a[-2], 2);
    EXPECT_EQ(a[-3], 1);
}

TEST(Vector, Swap) {
    var a = vector(0, 1, 2);
    var b = vector(3, 4, 5);

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
    var a = vector(0, 1, 2);
    var b = move(a);
    EXPECT_EQ(b[0], 0);
    EXPECT_EQ(b[1], 1);
    EXPECT_EQ(b[2], 2);
}

TEST(Vector, Reserve) {
    var a = vector(1, 10);
    EXPECT_EQ(a.len(), 2);
    EXPECT_EQ(a.size(), 2);

    a.insert(0, 10);
    EXPECT_EQ(a.len(), 3);
    EXPECT_EQ(a.size(), 3);

    a.reserve(10);
    EXPECT_EQ(a.len(), 3);
    EXPECT_EQ(a.size(), 10);
    EXPECT_EQ(a[0], 10);

    var b = vector_reserve<float>(10);
    EXPECT_EQ(b.size(), 10);
}

TEST(Vector, Insert) {
    var a = vector(0, 2, 3, 4);
    a.insert(1, 1);
    EXPECT_EQ(a, range(5));

    var b = vector(0, 1, 2, 4);
    b.insert(-1, 3);
    EXPECT_EQ(b, range(5));
}

TEST(Vector, String) {
    var a = vector<String<8>>("123", "456");

    EXPECT_EQ(a[0], "123");
    EXPECT_EQ(a[1], "456");
}
