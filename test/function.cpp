#include "gtest/gtest.h"
#include "etl/function.h"

using namespace Project::etl;

TEST(Function, Void) {
    Function<void(int &)> square;

    int i = 4;
    square(i);
    EXPECT_EQ(i, 4);

    square = [](auto, int& val) { val *= val; };
    square(i);
    EXPECT_EQ(i, 16);
}