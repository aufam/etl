#include "gtest/gtest.h"
#include "etl/array.h"
#include "etl/vector.h"
#include "etl/linked_list.h"
#include "etl/algorithm.h"
#include "etl/function.h"
#include "etl/keywords.h"

using namespace Project::etl;

TEST(Utility, enumerate) {
    var p = array(10, 11, 12);
    var i = 0; // loop guard
    for (val [x, y] in enumerate(p, 10)) {
        EXPECT_EQ(x, y);
        y -= 10;
        if (i++ == 5) break;
    }
    EXPECT_TRUE(compare_all(p, array(0, 1, 2)));
}

TEST(Utility, zip) {
    var p = vector(2, 4, 6, 8);
    constexpr val q = array(1, 2, 3);
    int i = 0; // loop guard
    for (var [x, y] in zip(p, q)) {
        EXPECT_EQ(x / y, 2);
        x = y;
        if (i++ == 5) break;
    }
    i = 0;
    for (val [x, y] in zip(p, q)) {
        EXPECT_EQ(x, y);
        if (i++ == 5) break;
    }

    constexpr val a = array(1, 2, 3);
    constexpr val b = array(2, 4, 6);
    constexpr val f = [](const decltype(a)& a, const decltype(b)& b) {
        int res = 0, i = 0;
        for (val [x, y] in zip(a, b)) {
            res += x * y;
            if (i++ == 5) break; // loop guard
        }
        return res;
    };
    constexpr val g = f(a,b);
    EXPECT_EQ(g, 28);
}

TEST(Utility, range) {
    var q = vector<int>();
    int j = 0; // loop guard
    for (auto i in range(3)) {
        q.append(i);
        if (j++ == 5) break;
    }
    EXPECT_TRUE(compare_all(q, array(0, 1, 2)));
    EXPECT_EQ(q.len(), 3);
}

TEST(Utility, Next) {
    var a = range(0, 3, 1);
    EXPECT_EQ(next(a), 0);
    EXPECT_EQ(next(a), 1);
    EXPECT_EQ(next(a), 2);
    EXPECT_EQ(next(a), 3); // goes beyond the limit
    EXPECT_EQ(next(a), 4);
}

TEST(Utility, Iter) {
    val a = array(1, 2, 3);
    var i = iter(a);
    EXPECT_EQ(next(i), 1);
    EXPECT_EQ(next(i), 2);
    EXPECT_EQ(next(i), 3);

    val v = vector(4, 5, 6);
    var j = iter(v);
    EXPECT_EQ(next(j), 4);
    EXPECT_EQ(next(j), 5);
    EXPECT_EQ(next(j), 6);

    val l = list(7, 8, 9);
    var k = iter(l);
    EXPECT_EQ(next(k), 7);
    EXPECT_EQ(next(k), 8);
    EXPECT_EQ(next(k), 9);
}

TEST(Utility, Generator) {
    var num = array(0, 1);
    val fib = Function<int(), decltype(num)&>( [](auto num) {
        int temp = num[1];
        num[1] += num[0];
        num[0] = temp;
        return temp;
    }, num);

    const int fibonacci_sequence[10] = {1, 1, 2, 3, 5, 8, 13, 21, 34, 55 };
    EXPECT_TRUE(all_of(fibonacci_sequence, fib));
}
