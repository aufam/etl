#include "gtest/gtest.h"
#include "etl/function.h"

using namespace Project::etl;

TEST(Function, Context) {
    int i = 2;
    /// equivalent to:
    /// ```C++
    /// std::function<void()> square = [&i]{ i *= i; };
    /// ```
    Function<void(), int&> square = { [](int& i) { i *= i; }, i };
    square();
    EXPECT_EQ(i, 4);

    square.fn = nullptr;
    square(); // do nothing
    EXPECT_EQ(i, 4);
}

TEST(Function, ContextVoid) {
    /// equivalent to:
    /// ```C++
    /// std::function<int(int)> square = [](int i){ return i *= i; };
    /// ```
    Function<int(int)> square = {[](int i) { return i * i; }};
    EXPECT_EQ(square(2), 4);
}