#include "gtest/gtest.h"
#include "etl/tuple.h"
#include "etl/python_keywords.h"

using namespace Project::etl;

TEST(Tuple, tuple) {
    Tuple<float, int, double> t = {0.1f, 2, 0.3};
    get<0>(t) = 0.5f;
    get<1>(t) = 5;
    get<2>(t) = 0.5;

    EXPECT_EQ(get<0>(t), 0.5f);
    EXPECT_EQ(get<1>(t), 5);
    EXPECT_EQ(get<2>(t), 0.5);

    // not working
    // auto [a, b, c] = t;
}

TEST(Tuple, TypeDeduced) {
    auto a = tuple(0.1f, 2, 0.3);
    auto t = tuple(1);
    EXPECT_EQ(get<0>(a), 0.1f);
    EXPECT_EQ(get<1>(a), 2);
    EXPECT_EQ(get<2>(a), 0.3);
}