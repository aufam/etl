#include "etl/placeholder.h"
#include "etl/vector.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project::etl;

TEST(Placeholder, Arithmetic) {
    val static constexpr multiplyBy2 = placeholder::arg * 2;
    EXPECT_EQ(multiplyBy2(3), 6);

    val static constexpr multiplyBy8 = multiplyBy2 * 4;
    EXPECT_EQ(multiplyBy8(3), 24);

    val plus2ThenMultiplyBy5 = (placeholder::arg + 2) * 5;
    EXPECT_EQ(plus2ThenMultiplyBy5(8), 50);
}

TEST(Placeholder, Reference) {
    int a = 10;
    val aMultiplyBy5 = placeholder::arg *= 5;
    aMultiplyBy5(a);
    EXPECT_EQ(a, 50);

    val aInc = ++placeholder::arg;
    aInc(a);
    EXPECT_EQ(a, 51);

    val aDec = placeholder::arg--;
    val b = aDec(a);
    EXPECT_EQ(a, 50);
    EXPECT_EQ(b, 51);
}


TEST(Placeholder, Assignment) {
    int a = 10;
    val assign5 = placeholder::arg = 5;
    assign5(a);
    EXPECT_EQ(a, 5);

    var v = vector<int>(1, 2, 3);
    var b = vector<int>();
    val assign_v = placeholder::arg = move(v);
    assign_v(b);
    EXPECT_EQ(v.len(), 0);
    EXPECT_EQ(b, vector(1, 2, 3));
}