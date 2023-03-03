#include <cstddef>
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
}

TEST(Tuple, TypeDeduced) {
    auto a = tuple(0.1f, 2, 0.3);
    EXPECT_EQ(get<0>(a), 0.1f);
    EXPECT_EQ(get<1>(a), 2);
    EXPECT_EQ(get<2>(a), 0.3);
}


TEST(Tuple, StructureBinding) {
    const auto [a, b, c] = tuple(1, 0.2f, 0.3);
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 0.2f);
    EXPECT_EQ(c, 0.3);
}
