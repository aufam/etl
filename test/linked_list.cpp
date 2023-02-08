#include "gtest/gtest.h"
#include "etl/all.h"

using namespace Project::etl;

TEST(LinkedList, Push) {
    const LinkedList<int> list;
    list << 1 << 3 << 4;
    list.push(2, 1);
    list.pushFront(0);

    for (int i = 0; i < 5; ++i) EXPECT_EQ(list[i], i);
    EXPECT_EQ(list.len(), 5);
}

TEST(LinkedList, Pop) {
    const LinkedList<int> list;
    list << 0 << 1 << 2 << 3 << 4;

    int item;
    for (int i = 0; i < 5; ++i) {
        list >> item;
        EXPECT_EQ(item, i);
    }
    EXPECT_EQ(list.len(), 0);

    list << 0 << 1 << 2 << 3 << 4;
    list.pop(item, 1);
    EXPECT_EQ(item, 1);
}

TEST(LinkedList, ForLoop) {
    const LinkedList<int> list;
    int res[] = {1, 2, 3, 4, 5};
    for (auto item : res) list << item;

    // forward
    int *p = res;
    for (auto item : list) EXPECT_EQ(item, *p++);

    //backward
    p = &res[4];
    for (auto pItem = list.tail(); pItem; pItem--) EXPECT_EQ(*pItem, *p--);

    EXPECT_EQ(list.len(), 5);
}

TEST(LinkedList, Iterator) {
    const LinkedList<int> list;
    int res[] = {1, 2, 3, 4, 5};
    for (auto item : res) list << item;

    auto iter1 = list.begin();
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

    iter1 = list.tail();
    EXPECT_EQ(res[4], *iter1);
    EXPECT_EQ(res[3], iter1[-1]);

    EXPECT_EQ(list.len(), 5);
}


TEST(LinkedList, Non_constant) {
    LinkedList<int> list;
    for (int i = 0; i < 5; i++) list[i] = i;
    list.front() = 100;
    list.back() = 200;

    EXPECT_EQ(100, list[0]);
    EXPECT_EQ(1, list[1]);
    EXPECT_EQ(2, list[2]);
    EXPECT_EQ(3, list[3]);
    EXPECT_EQ(200, list[4]);
    EXPECT_EQ(list.len(), 5);
}

TEST(LinkedList, Fold) {
    const LinkedList<int> list;
    list << 1 << 2 << 3 << 4 << 5;

    int sum = 0;
    fold(list.begin(), list.end(), [](auto& item, auto& res) { res += item; }, sum);
    EXPECT_EQ(sum, 15);

    int mul = 1;
    fold(list.begin(), list.end(), [](auto& item, auto& res) { res *= item; }, mul);
    EXPECT_EQ(mul, 120);

    EXPECT_EQ(list.len(), 5);
}