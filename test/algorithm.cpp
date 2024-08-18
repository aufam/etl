#include "etl/algorithm.h"
#include "etl/array.h"
#include "etl/function.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project::etl;

TEST(Algorithm, Find) {
    val a = {1, 2, 3};
    EXPECT_EQ(*find(a, 3), 3);
    EXPECT_EQ(*find_if(a, lambda (val &item) { return item == 3; }), 3);
    EXPECT_EQ(*find_if_not(a, lambda (val &item) { return item != 3; }), 3);
}

TEST(Algorithm, AllAnyNone) {
    val check = lambda (val item) { return item == 3; };
    val a = {3, 3, 3};
    EXPECT_TRUE (all_if(a, check));
    EXPECT_TRUE (any_if(a, check));
    EXPECT_FALSE(none_if(a, check));

    val b = {1, 2, 3};
    EXPECT_FALSE(all_if(b, check));
    EXPECT_TRUE (any_if(b, check));
    EXPECT_FALSE(none_if(b, check));
    EXPECT_TRUE (all_of(b, range(1, 4)));
    EXPECT_TRUE (any_of(b, range(1, 4)));
    EXPECT_FALSE(none_of(b, range(1, 4)));

#if __cplusplus == 201703L
    EXPECT_FALSE(compare_all(a, b));
    EXPECT_TRUE (compare_any(a, b));
    EXPECT_FALSE(compare_none(a, b));
#endif
}

TEST(Algorithm, ForeachFold) {
    val a = {1, 2, 3, 4};
    var res = 0;
    
    fold(a, lambda (val item, var &res) { res += item; }, res);
    EXPECT_EQ(res, 10);

    res = 1;
    fold(a, lambda (val item, var &res) { res *= item; }, res);
    EXPECT_EQ(res, 24);

    val b = {4, 4, 4, 4};
    foreach(b, lambda (val item) { EXPECT_EQ(item, 4); });
}

TEST(Algorithm, FillGenerate) {
    val constexpr makeArray = lambda (int item) {
        var res = array<int, 3>();
        fill(res, item);
        return res;
    };
    val a = makeArray(3);
    EXPECT_TRUE(all_if(a, lambda (val item) { return item == 3; }));

    int b[3];
    generate(b, range(3));
    EXPECT_TRUE(all_of(b, range(3)));
}

TEST(Algorithm, Count) {
    val a = {1, 1, 1, 4};
    EXPECT_EQ(count(a, 1), 3);
    EXPECT_EQ(count_if(a, lambda (val &item) { return item == 1; }), 3);
}

#if __cplusplus == 201703L
TEST(Algorithm, CopyReplace) {
    var a = array<int, 3>();
    val b = {0, 1, 2};
    val c = {10, 1, 2};
    val d = {10, 20, 20};

    copy(range(3), a);
    EXPECT_EQ(a, b);

    fill(a, 0);
    copy_if(range(10), a, lambda (val item) { return item < 3; });
    EXPECT_EQ(a, b);

    replace(a, 0, 10);
    EXPECT_EQ(a, c);

    replace_if(a, lambda (val item) { return item < 10; }, 20);
    EXPECT_EQ(a, d);
}
#endif

TEST(Algorithm, MaxMinSumClamp) {
    val a = {1, 2, 3};
    EXPECT_EQ(min_element(a), 1);
    EXPECT_EQ(max_element(a), 3);
    EXPECT_EQ(sum_element(a), 6);

    EXPECT_EQ(min(1, 2, 3), 1);
    EXPECT_EQ(max(1, 2, 3), 3);
    EXPECT_EQ(sum(1, 2, 3), 6);

    EXPECT_EQ(clamp(2, 1, 3), 2);
    EXPECT_EQ(clamp(0, 1, 3), 1);
    EXPECT_EQ(clamp(4, 1, 3), 3);
}
