#include "gtest/gtest.h"
#include "etl/array.h"
#include "etl/algorithm.h"

using namespace Project::etl;

TEST(Array, Empty) {
    Array<int, 5> array = {};
    foreach(array.begin(), array.end(), [](auto& item) { EXPECT_EQ(item, 0); });
}