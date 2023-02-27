#include "gtest/gtest.h"
#include "etl/function.h"

using namespace Project::etl;

TEST(Function, Void) {
    Function<void(int &)> square;

    int i = 4;
    square(i); // do nothing
    EXPECT_EQ(i, 4);

    // no context
    square = [](auto, int& val) { val *= val; };
    square(i);
    EXPECT_EQ(i, 16);

    // with context
    square = {[](auto context, auto) {
        int& i = *reinterpret_cast<int*>(context);
        i *= i;
    }, &i};
    int dummy =10000;
    square(dummy);
    EXPECT_EQ(i, 256);
}
