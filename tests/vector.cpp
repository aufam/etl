#include "etl/vector.h"
#include "etl/string.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project::etl;

TEST(Vector, Declaration) {
    val a = vector(0, 1, 2);                // using variadic function
    val b = vector<int>(0.0, 1.0f, '\02');  // implicitly cast to the desired type
    val c = vector({0, 1, 2});              // from initializer list
    val d = vector<int>();                  // empty vector

    val x = vector(0, 1, 2);
    EXPECT_EQ(a, x);
    EXPECT_EQ(b, x);
    EXPECT_EQ(c, x);
    EXPECT_FALSE(d == x);
}

TEST(Vector, Append) {
    var a = vector(0, 1, 2);
    val b = vector(3, 4, 5);

    val c = a + b;
    EXPECT_EQ(c, vectorize(range(6)));

    a += b;
    EXPECT_EQ(a, vectorize(range(6)));
}

TEST(Vector, Remove) {
    var a = vector(0, 1, 2, 3, 4, 5, 5);
    EXPECT_EQ(len(a), 7);
    EXPECT_EQ(a.back(), 5);

    a.remove(5);
    EXPECT_EQ(len(a), 6);
    EXPECT_EQ(a, vectorize(range(6)));

    a.remove_at(-1); // remove last item
    EXPECT_EQ(len(a), 5);
    EXPECT_EQ(a, vectorize(range(5)));
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

    EXPECT_FALSE(a);
    EXPECT_EQ(b[0], 0);
    EXPECT_EQ(b[1], 1);
    EXPECT_EQ(b[2], 2);

    a = vector(3, 4, 5);
    b += move(a);

    EXPECT_FALSE(a);
    EXPECT_EQ(b[0], 0);
    EXPECT_EQ(b[1], 1);
    EXPECT_EQ(b[2], 2);
    EXPECT_EQ(b[3], 3);
    EXPECT_EQ(b[4], 4);
    EXPECT_EQ(b[5], 5);
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
    EXPECT_EQ(a, vectorize(range(5)));

    var b = vector(0, 1, 2, 4);
    b.insert(-1, 3);
    EXPECT_EQ(b, vectorize(range(5)));
}

TEST(Vector, String) {
    var a = vector<String<8>>("123", "456");

    EXPECT_EQ(a[0], "123");
    EXPECT_EQ(a[1], "456");
}

TEST(Vector, Resize) {
    int num = 10;
    var a = vector(num, 2, 3, 4);
    a.reserve(5);
    a.resize(2);
    EXPECT_EQ(a.len(), 2);
    EXPECT_EQ(a.size(), 5);
    EXPECT_EQ(a, vector(10, 2));

    a.fill(0);
    EXPECT_EQ(a, vector(10, 2, 0, 0, 0));
}

TEST(Vector, Vectorize) {
    val a = vectorize(range(10));
    EXPECT_EQ(a, vectorize(range(10)));

    val b = vectorize(a | transform(lambda (val item) { return item * 2; }));
    EXPECT_EQ(b, vectorize(range(0, 20, 2)));

    var v = vector(1, 2, 3);
    val c = vectorize(v); // copy
    EXPECT_EQ(v, vectorize(range(1, 4)));
    EXPECT_EQ(c, vectorize(range(1, 4)));

    val d = vectorize(move(v)); // move
    EXPECT_FALSE(v);
    EXPECT_EQ(d, vectorize(range(1, 4)));
}
