#include "etl/array.h"
#include "etl/vector.h"
#include "etl/linked_list.h"
#include "etl/bit.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project::etl;

TEST(Utility, Iter) {
    [[maybe_unused]] constexpr bool asjfdbi = trait_reverse_iterable<Vector<int>>::value;
    val a = array(1, 2, 3);
    var i = iter(a);
    EXPECT_EQ(next(i), 1);
    EXPECT_EQ(next(i), 2);
    EXPECT_EQ(next(i), 3);

    i = reversed(a);
    EXPECT_EQ(next(i), 3);
    EXPECT_EQ(next(i), 2);
    EXPECT_EQ(next(i), 1);

    val v = vector(4, 5, 6);
    var j = reversed(v);
    EXPECT_EQ(next(j), 6);
    EXPECT_EQ(next(j), 5);
    EXPECT_EQ(next(j), 4);

    j = reversed(v);
    EXPECT_EQ(next(j), 6);
    EXPECT_EQ(next(j), 5);
    EXPECT_EQ(next(j), 4);

    val l = list(7, 8, 9);
    var k = iter(l);
    EXPECT_EQ(next(k), 7);
    EXPECT_EQ(next(k), 8);
    EXPECT_EQ(next(k), 9);

    k = reversed(l);
    EXPECT_EQ(next(k), 9);
    EXPECT_EQ(next(k), 8);
    EXPECT_EQ(next(k), 7);
}

TEST(Utility, range) {
    var a = vector<int>();
    for (val i in range(3))
        a.append(i);

    EXPECT_EQ(a, array(0, 1, 2));

    var b = vector<int>();
    for (val i in range(3, 0, -1)) 
        b.append(i);

    EXPECT_EQ(b, array(3, 2, 1));

    val r1 = range(10);
    EXPECT_EQ(reversed(r1), range(9, -1, -1));
}

TEST(Utility, enumerate) {
    var p = array(10, 11, 12);
    for (var [x, y] in enumerate(p, 10)) {
        EXPECT_EQ(x, y);
        y -= 10;
    }

    EXPECT_EQ(p, array(0, 1, 2));
}

TEST(Utility, zip) {
    var p = vector(2, 4, 6, 8);
    val constexpr q = array(1, 2, 3);
    
    for (var [x, y] in zip(p, q)) {
        EXPECT_EQ(x / y, 2);
        x = y;
    }

    for (var [x, y] in zip(p, q))
        EXPECT_EQ(x, y);
    
    EXPECT_EQ(p, array(1, 2, 3, 8));

    val constexpr a = array(1, 2, 3);
    val constexpr b = array(2, 4, 6);
    val constexpr f = lambda (val &a, val &b) {
        var res = 0;
        for (var [x, y] in zip(a, b))
            res += x * y;
        return res;
    };
    val constexpr g = f(a,b);
    EXPECT_EQ(g, 28);
}

TEST(Utility, Next) {
    var a = range(3);
    EXPECT_EQ(next(a), 0);
    EXPECT_EQ(next(a), 1);
    EXPECT_EQ(next(a), 2);
    EXPECT_EQ(next(a), 0); // out of limit, return default value
    EXPECT_EQ(next(a), 0);
}

TEST(Utility, Generator) {
    // a generator is any object that has operator()
    var num = array(0, 1);
    val fib = [&num] {
        val temp = num[1];
        num[1] += num[0];
        num[0] = temp;
        return temp;
    };

    val fibonacci_sequence = { 1, 1, 2, 3, 5, 8, 13, 21, 34, 55 };
    EXPECT_TRUE(all_of(fibonacci_sequence, fib));
}

TEST(Utility, Transform) {
    val a = array(1, 2, 3);
    val b = a | transform(lambda (val item) { return item * item; });
    EXPECT_EQ(array(1, 4, 9), vectorize(b));

    val v = vector(1, 2, 3);
    val c = v | transform(lambda (val item) { return item * item; });
    EXPECT_EQ(array(1, 4, 9), vectorize(c));

    val l = list(1, 2, 3);
    val d = l | transform(lambda (val item) { return item * item; });
    EXPECT_EQ(array(1, 4, 9), vectorize(d));

    val r = range(1, 4);
    val e = r | transform(lambda (val item) { return item * item; });
    EXPECT_EQ(array(1, 4, 9), vectorize(e));
}

TEST(Utility, Filter) {
    val a = array(1, 2, 3, 4, 5, 6);
    val x = a | filter(lambda (val item) { return is_even(item); });
    EXPECT_EQ(array(2, 4, 6), vectorize(x));

    val d = range(1, 7);
    val r = d | filter(lambda (val item) { return is_even(item); });
    EXPECT_EQ(array(2, 4, 6), vectorize(r));

    var s = 2;
    for (val i in array(1,2,3,4,5,6) | filter(lambda (val item) { return is_even(item); })) {
        EXPECT_EQ(s, i);
        s += 2;
    }

    s = 1;
    for (val i in array(1,2,3,4,5,6) | filter(lambda (val) { return true; })) {
        EXPECT_EQ(s, i);
        s++;
    }
}
