#include "gtest/gtest.h"
#include "etl/array.h"
#include "etl/vector.h"
#include "etl/linked_list.h"
#include "etl/algorithm.h"
#include "etl/function.h"
#include "etl/python_keywords.h"

using namespace Project::etl;

TEST(Utility, enumerate) {
    auto p = array(10, 11, 12);
    int i = 0; // loop guard
    for (auto [x, y] in enumerate(p, 10)) {
        EXPECT_EQ(x, y);
        y -= 10;
        if (i++ == 5) break;
    }
    EXPECT_TRUE(compare_all(p, array(0, 1, 2)));
}

TEST(Utility, zip) {
    auto p = vector(2, 4, 6, 8);
    constexpr auto q = array(1, 2, 3);
    int i = 0; // loop guard
    for (auto [x, y] in zip(p, q)) {
        EXPECT_EQ(x / y, 2);
        x = y;
        if (i++ == 5) break;
    }
    EXPECT_TRUE(compare_all(p, q));

    constexpr auto a = array(1, 2, 3);
    constexpr auto b = array(2, 4, 6);
    constexpr auto f = [](const Array<int, 3>& a, const Array<int, 3>& b) {
        int res = 0, i = 0;
        for (auto [x, y] in zip(a, b)) {
            res += x * y;
            if (i++ == 5) break; // loop guard
        }
        return res;
    };
    constexpr auto g = f(a,b);
    EXPECT_EQ(g, 28);
}

TEST(Utility, range) {
    Vector<int> q;
    int j = 0; // loop guard
    for (auto i in range(3)) {
        q.append(i);
        if (j++ == 5) break;
    }
    EXPECT_TRUE(compare_all(q, array(0, 1, 2)));
    EXPECT_EQ(q.len(), 3);
}

TEST(Utility, Next) {
    auto a = range(0, 3, 1);
    EXPECT_EQ(next(a), 0);
    EXPECT_EQ(next(a), 1);
    EXPECT_EQ(next(a), 2);
    EXPECT_EQ(next(a), 3); // goes beyond the limit
    EXPECT_EQ(next(a), 4);
}

TEST(Utility, Iter) {
    auto a = array(1, 2, 3);
    auto i = iter(a);
    EXPECT_EQ(next(i), 1);
    EXPECT_EQ(next(i), 2);
    EXPECT_EQ(next(i), 3);

    auto v = vector(4, 5, 6);
    auto j = iter(v);
    EXPECT_EQ(next(j), 4);
    EXPECT_EQ(next(j), 5);
    EXPECT_EQ(next(j), 6);

    auto l = list(7, 8, 9);
    auto k = iter(l);
    EXPECT_EQ(next(k), 7);
    EXPECT_EQ(next(k), 8);
    EXPECT_EQ(next(k), 9);
}

TEST(Utility, Generator) {
    auto num = array(0, 1);
    auto fib = Function<int(), decltype(num)&>( [](auto num) {
        int temp = num[1];
        num[1] += num[0];
        num[0] = temp;
        return temp;
    }, num);

    const int fibonacci_sequence[10] = {1, 1, 2, 3, 5, 8, 13, 21, 34, 55 };
    EXPECT_TRUE(all_of(fibonacci_sequence, fib));
}