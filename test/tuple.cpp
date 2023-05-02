#include "etl/tuple.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project::etl;

TEST(Tuple, Declaration) {
    val a = tuple(0.1f, 2, 0.3); // types are implicitly specified
    EXPECT_EQ(get<0>(a), 0.1f);
    EXPECT_EQ(get<1>(a), 2);
    EXPECT_EQ(get<2>(a), 0.3);

    val b = tuple<int, float, double>(0.1f, 2, 0.3); // types are explicitly specified
    EXPECT_EQ(get<0>(b), 0);
    EXPECT_EQ(get<1>(b), 2.0f);
    EXPECT_EQ(get<2>(b), 0.3);
}

TEST(Tuple, Modification) {
    var t = tuple(0.1f, 2, 0.3);
    get<0>(t) = 0.5f;
    get<1>(t) = 5;
    get<2>(t) = 0.5;

    EXPECT_EQ(get<0>(t), 0.5f);
    EXPECT_EQ(get<1>(t), 5);
    EXPECT_EQ(get<2>(t), 0.5);
    EXPECT_EQ(len(t), 3);
}

TEST(Tuple, StructureBinding) {
    val [a, b, c] = tuple(1, 0.2f, 0.3);
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 0.2f);
    EXPECT_EQ(c, 0.3);
}

TEST(Tuple, Compare) {
    val a = tuple(1, 0.3);
    val b = tuple(1, 0.3);
    EXPECT_EQ(a, b);
    EXPECT_EQ(len(a), 2);
    EXPECT_EQ(len(b), 2);

    val c = tuple(1, 2.0f, 3.0);
    val d = triple(1, 2.0f, 3.0);
    EXPECT_EQ(c, d);
    EXPECT_EQ(d, c);
    EXPECT_EQ(len(c), 3);
    EXPECT_EQ(len(d), 3);
}

TEST(Tuple, Slice) {
    val a = tuple(1, 0.3, 1, 0.3f);
    val b = get<1, 3>(a);
    EXPECT_EQ(get<0>(b), 0.3);
    EXPECT_EQ(get<1>(b), 1);
}
