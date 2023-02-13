#include "gtest/gtest.h"
#include "etl/algorithm.h"
#include "etl/array.h"

using namespace Project::etl;

TEST(Algorithm, Find) {
    const int a[3] = { 1, 2, 3};
    EXPECT_EQ(*find(a, 3), 3);
    EXPECT_EQ(*find_if(a, [](auto& item) { return item == 3; }), 3);
    EXPECT_EQ(*find_if_not(a, [](auto& item) { return item != 3; }), 3);
}

TEST(Algorithm, All_Any_None) {
    auto check = [](const int& item) { return item == 3; };
    const int a[3] = { 3, 3, 3};
    EXPECT_TRUE (all(a, 3));
    EXPECT_TRUE (any(a, 3));
    EXPECT_FALSE(none(a, 3));
    EXPECT_TRUE (all_if(a, check));
    EXPECT_TRUE (any_if(a, check));
    EXPECT_FALSE(none_if(a, check));

    const int b[3] = { 1, 2, 3};
    EXPECT_FALSE(all(b, 3));
    EXPECT_TRUE (any(b, 3));
    EXPECT_FALSE(none(b, 3));
    EXPECT_FALSE(all_if(b, check));
    EXPECT_TRUE (any_if(b, check));
    EXPECT_FALSE(none_if(b, check));

    EXPECT_FALSE(compare_all(a, b));
    EXPECT_TRUE (compare_any(a, b));
    EXPECT_FALSE(compare_none(a, b));
}

TEST(Algorithm, Foreach_Fold) {
    const int a[4] = { 1, 2, 3, 4};
    int res = 0;
    fold(a, [](auto& item, auto& res) { res += item; }, res);
    EXPECT_EQ(res, 10);
    res = 1;
    fold(a, [](auto& item, auto& res) { res *= item; }, res);
    EXPECT_EQ(res, 24);

    const int b[4] = { 4, 4, 4, 4};
    foreach(b, [](auto& item) { EXPECT_EQ(item, 4); });
}

TEST(Algorithm, Fill_Generate) {
    constexpr auto makeArray = [](int val) {
        Array<int, 3> res = {};
        fill(res, val);
        return res;
    };
    const auto a = makeArray(3);
    EXPECT_TRUE(all(a, 3));

    int b[3];
    generate(b, range(3));
    EXPECT_TRUE(compare_all(b, Array<int, 3> {0, 1, 2}));
}

TEST(Algorithm, Count) {
    constexpr int a[4] = { 1, 1, 1, 4};
    EXPECT_EQ(count(a, 1), 3);
    EXPECT_EQ(count_if(a, [](auto& item) { return item == 1; }), 3);
}