#include "etl/linked_list.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project::etl;

TEST(LinkedList, Declaration) {
    val a = list(0, 1, 2);                 // using variadic function
    val b = list<int>(0.0, 1.0f, '\02');   // implicitly cast to the desired type
    val c = list({0, 1, 2});               // from initializer list
    val d = list<int>();                   // empty list

    val x = {0, 1, 2};
    EXPECT_EQ(a, x);
    EXPECT_EQ(b, x);
    EXPECT_EQ(c, x);
    EXPECT_EQ(len(a), 3);
    EXPECT_EQ(len(b), 3);
    EXPECT_EQ(len(c), 3);
    EXPECT_EQ(len(d), 0);
}

TEST(LinkedList, Push) {
    val a = list<int>();
    a << 1 << 3 << 4;
    a.push(2, 1);
    a.pushFront(0);

    EXPECT_EQ(a, range(5));
    EXPECT_EQ(len(a), 5);
}

TEST(LinkedList, Pop) {
    val a = list<int>(0, 1, 2, 3, 4);
    EXPECT_EQ(len(a), 5);

    int item;
    for (val i in range(5)) {
        a >> item;
        EXPECT_EQ(item, i);
    }
    EXPECT_EQ(len(a), 0);

    a << 0 << 100 << 2 << 3 << 4;
    a.pop(item, 1);
    EXPECT_EQ(item, 100);
}

TEST(LinkedList, ForLoop) {
    val a = list<int>();
    int res[] = {1, 2, 3, 4, 5};
    for (val item in res) a << item;

    // forward
    int *p = res;
    for (val item in a) EXPECT_EQ(item, *p++);

    //backward
    p = &res[4];
    for (var pItem = a.tail(); pItem; pItem--) EXPECT_EQ(*pItem, *p--);

    EXPECT_EQ(a.len(), 5);
}

TEST(LinkedList, Iterator) {
    val a = list<int>();
    int res[] = {1, 2, 3, 4, 5};
    for (val item in res) a << item;

    var iter1 = a.begin();
    ++iter1;
    EXPECT_EQ(res[1], *iter1);

    var iter2 = iter1++;
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

    var b = a.begin() + 1;
    var c = a.begin() + 4;
    EXPECT_EQ(c - b, 3);

    var d = a.end();
    EXPECT_EQ(d - b, 4);

    var e = a.begin();
    EXPECT_EQ(b - e, 1);

    EXPECT_EQ(a.len(), 5);
}

TEST(LinkedList, Iter) {
    var a = list(1, 2, 3);
    // var b = iter(a.tail(), a.end(), -1);
    var b = reversed(a);
    EXPECT_EQ(b.len(), 3);
    EXPECT_EQ(b(), 3);
    EXPECT_EQ(b(), 2);
    EXPECT_EQ(b(), 1);

    var c = a(1, 3);
    EXPECT_EQ(c.len(), 2);
    EXPECT_EQ(c(), 2);
    EXPECT_EQ(c(), 3);
    EXPECT_EQ(c(), 0); // invalid
} 

TEST(LinkedList, Mutable) {
    var a = list<int>();
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

TEST(LinkedList, Indexing) {
    val a = list(1, 2, 3);
    EXPECT_EQ(a[-1], 3); // last item
    EXPECT_EQ(a[-2], 2);
    EXPECT_EQ(a[-3], 1);
}

TEST(LinkedList, Swap) {
    var a = list(0, 1, 2);
    var b = list(3, 4, 5);
    swap_element(a, b); // swap each element
    EXPECT_EQ(b[0], 0);
    EXPECT_EQ(b[1], 1);
    EXPECT_EQ(b[2], 2);
    EXPECT_EQ(a[0], 3);
    EXPECT_EQ(a[1], 4);
    EXPECT_EQ(a[2], 5);
    swap(a, b); // swap linked list
    EXPECT_EQ(a[0], 0);
    EXPECT_EQ(a[1], 1);
    EXPECT_EQ(a[2], 2);
    EXPECT_EQ(b[0], 3);
    EXPECT_EQ(b[1], 4);
    EXPECT_EQ(b[2], 5);
}

TEST(LinkedList, Move) {
    var a = list(0, 1, 2);
    var b = move(a);

    EXPECT_EQ(len(a), 0);
    EXPECT_EQ(a.begin(), null);

    EXPECT_EQ(len(b), 3);
    EXPECT_EQ(b[0], 0);
    EXPECT_EQ(b[1], 1);
    EXPECT_EQ(b[2], 2);
}