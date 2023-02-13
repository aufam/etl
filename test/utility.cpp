#include "gtest/gtest.h"
#include "etl/array.h"
#include "etl/vector.h"
#include "etl/algorithm.h"
#include "etl/function.h"
#include "etl/python_keywords.h"

using namespace Project::etl;

TEST(Utility, enumerate) {
    using array = Array<int, 3>;
    array p = { 10, 11, 12 };

    bool sdf = 1 is_not 10;

    int i = 0; // loop guard
    for (auto [x, y] in enumerate(p, 10)) {
        EXPECT_EQ(x, y);
        y -= 10;
        if (i++ == 5) break;
    }
    EXPECT_TRUE(compare_all(p, array{0, 1, 2}));
}

TEST(Utility, zip) {
    using vector = Vector<int>;
    using array = Array<int, 3>;

    vector p = { 2, 4, 6, 8 };
    constexpr array q = { 1, 2, 3 };

    int i = 0; // break guard
    for (auto [x, y] in zip(p, q)) {
        EXPECT_EQ(x / y, 2);
        x = y;
        if (i++ == 5) break;
    }

    EXPECT_TRUE(compare_all(p, q));

    constexpr Array<int, 3> a = { 1, 2, 3};
    constexpr Array<int, 3> b = { 2, 4, 6};

    constexpr auto f = [](const Array<int, 3> & a, const Array<int, 3> & b) {
        int res = 0, i = 0;
        for (auto [x, y] in zip(a, b)) {
            res += x * y;
            if (i++ == 5) break;
        }
        return res;
    };

    constexpr auto g = f(a,b);
    EXPECT_EQ(g, 28);
}

TEST(Utility, range) {
    using array = Array<int, 3>;
    using vector = Vector<int>;

    array p = {};
    generate(p, range(1, 0));
    EXPECT_TRUE(compare_all(p, array { 1, 0, 0 }));

    vector q;
    int j = 0;
    for (auto i in range(3)) {
        q.append(i);
        if (j++ == 5) break;
    }

    EXPECT_TRUE(compare_all(q, array { 0, 1, 2}));
    EXPECT_EQ(q.len(), 3);

    Pair<int> num = { 0, 1 };
    Function<int()> fib = { [](auto context) {
        auto& num = *reinterpret_cast<Pair<int>*>(context);
        int temp = num.y;
        num.y += num.x;
        num.x = temp;
        return temp;
    }, &num };

    const int fibonacci_sequence[10] = {1, 1, 2, 3, 5, 8, 13, 21, 34, 55 };
    int r[10];
    generate(r, fib);
    EXPECT_TRUE(compare_all(r, fibonacci_sequence));
}