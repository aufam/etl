#include "etl/array.h"
#include "etl/algorithm.h"
#include "gtest/gtest.h"

using namespace Project::etl;

TEST(Array, Empty) {
    Array<int, 5> array = {};
    foreach(array.begin(), array.end(), [](auto& item) { EXPECT_EQ(item, 0); });
}