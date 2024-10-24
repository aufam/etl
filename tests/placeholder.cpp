#include "etl/placeholder.h"
#include "etl/vector.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project::etl;

TEST(Placeholder, Arithmetic) {
    val static constexpr multiplyBy2 = placeholder::arg<1> * 2;
    EXPECT_EQ(multiplyBy2(3), 6);

    val static constexpr multiplyBy8 = multiplyBy2 * 4;
    EXPECT_EQ(multiplyBy8(3), 24);

    val plus2ThenMultiplyBy5 = (placeholder::arg<1> + 2) * 5;
    EXPECT_EQ(plus2ThenMultiplyBy5(8), 50);
}

TEST(Placeholder, Reference) {
    int a = 10;
    val aMultiplyBy5 = placeholder::_1 *= 5;
    aMultiplyBy5(a);
    EXPECT_EQ(a, 50);

    val aInc = ++placeholder::_1;
    aInc(a);
    EXPECT_EQ(a, 51);

    val aDec = placeholder::_1--;
    val b = aDec(a);
    EXPECT_EQ(a, 50);
    EXPECT_EQ(b, 51);
}


TEST(Placeholder, Assignment) {
    int a = 10;
    val assign5 = placeholder::arg<1> = 5;
    assign5(a);
    EXPECT_EQ(a, 5);

    var v = vector<int>(1, 2, 3);
    var b = vector<int>();
    val assign_v = placeholder::arg<1> = move(v);
    assign_v(b);
    EXPECT_EQ(v.len(), 0);
#if __cplusplus == 201703L
    EXPECT_EQ(b, vector(1, 2, 3));
#endif
}

TEST(Placeholder, Equality) {
    using namespace placeholder;

    val foo = _1 == _2;
    EXPECT_FALSE(foo(3, 5));
}

TEST(Placeholder, Retval) {
    using namespace placeholder;
    
    val constexpr get5 = retval = 5;
    EXPECT_EQ(get5(), 5);

    val constexpr foo = retval = 5 + 20 * 3;
    EXPECT_EQ(foo(), 65);

    val constexpr bar = retval = _1 + _2;
    EXPECT_EQ(bar(2, 3), 5);

    int a = 10;
    val f = retval = _1;
    
    int& g = f(a);
    g++;

    EXPECT_EQ(a, 11);
}

TEST(Placeholder, Args) {
    using namespace placeholder;

    val foo = _1 + _2 + _3 + arg<5>;
    EXPECT_EQ(foo(3, 5, 6, 100, 2), 16);

    var v = vector(1, 2, 3);
    val bar = _2[_3] * 2 + _1;
    EXPECT_EQ(bar(1, move(v), 1), 5);
}