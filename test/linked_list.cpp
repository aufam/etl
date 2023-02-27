#include "gtest/gtest.h"
#include "etl/linked_list.h"
#include "etl/algorithm.h"
#include "etl/python_keywords.h"

using namespace Project::etl;

TEST(LinkedList, Push) {
    const LinkedList<int> a;
    a << 1 << 3 << 4;
    a.push(2, 1);
    a.pushFront(0);

    for (auto [x, y] in enumerate(a)) EXPECT_EQ(x, y);
    EXPECT_EQ(a.len(), 5);
}

TEST(LinkedList, Pop) {
    const LinkedList<int> a;
    a << 0 << 1 << 2 << 3 << 4;

    int item;
    for (int i in range(5)) {
        a >> item;
        EXPECT_EQ(item, i);
    }
    EXPECT_EQ(a.len(), 0);

    a << 0 << 1 << 2 << 3 << 4;
    a.pop(item, 1);
    EXPECT_EQ(item, 1);
}

TEST(LinkedList, ForLoop) {
    const LinkedList<int> a;
    int res[] = {1, 2, 3, 4, 5};
    for (auto item in res) a << item;

    // forward
    int *p = res;
    for (auto item in a) EXPECT_EQ(item, *p++);

    //backward
    p = &res[4];
    for (auto pItem = a.tail(); pItem; pItem--) EXPECT_EQ(*pItem, *p--);

    EXPECT_EQ(a.len(), 5);
}

TEST(LinkedList, Iterator) {
    const LinkedList<int> a;
    int res[] = {1, 2, 3, 4, 5};
    for (auto item in res) a << item;

    auto iter1 = a.begin();
    ++iter1;
    EXPECT_EQ(res[1], *iter1);

    auto iter2 = iter1++;
    EXPECT_EQ(res[1], *iter2);
    EXPECT_EQ(res[2], *iter1);

    --iter1;
    EXPECT_EQ(res[1], *iter1);

    iter2 = iter1--;
    EXPECT_EQ(res[0], *iter1);
    EXPECT_EQ(res[1], *iter2);

    iter2 -= 1;
    EXPECT_EQ(res[0], *iter2);

    iter1 = a.tail();
    EXPECT_EQ(res[4], *iter1);
    EXPECT_EQ(res[3], iter1[-1]);

    EXPECT_EQ(a.len(), 5);
}


TEST(LinkedList, Mutable) {
    LinkedList<int> a;
    for (int i = 0; i < 5; i++) a[i] = i;
    a.front() = 100;
    a.back() = 200;

    EXPECT_EQ(100, a[0]);
    EXPECT_EQ(1, a[1]);
    EXPECT_EQ(2, a[2]);
    EXPECT_EQ(3, a[3]);
    EXPECT_EQ(200, a[4]);
    EXPECT_EQ(a.len(), 5);
}

TEST(LinkedList, TypeDeduced) {
    const auto a = list(1, 2, 3, 4, 5);

    int sum = 0;
    fold(a.begin(), a.end(), [](auto& item, auto& res) { res += item; }, sum);
    EXPECT_EQ(sum, 15);

    int mul = 1;
    fold(a.begin(), a.end(), [](auto& item, auto& res) { res *= item; }, mul);
    EXPECT_EQ(mul, 120);

    EXPECT_EQ(a.len(), 5);
}