#include "etl/map.h"
#include "etl/string.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project::etl;
using namespace Project::etl::literals;

TEST(Map, Initialize) {
    var a = map<String<8>, int>(); // empty map
    EXPECT_EQ(a.begin(), null);
    EXPECT_EQ(a.len(), 0);
    EXPECT_EQ(a.size(), 0);

    var b = map_reserve<String<8>, int>(10); // empty map with initial capacity
    EXPECT_NE(b.begin(), null);
    EXPECT_EQ(b.len(), 0);
    EXPECT_EQ(b.size(), 10);

    val c = map(pair("one"s, 1), pair("two"s, 2), pair("three"s, 3)); // from variadic function
    EXPECT_NE(c.begin(), null);
    EXPECT_EQ(c.len(), 3);
    EXPECT_EQ(c.size(), 3);

    var d = map<String<8>, int>({{"one", 1}, {"two", 2}, {"three", 3}}); // from initializer_list
    EXPECT_NE(d.begin(), null);
    EXPECT_EQ(d.len(), 3);
    EXPECT_EQ(d.size(), 3);
}

TEST(Map, Remove) {
    var m = map<String<8>, int>();
    m["one"] = 1;
    m["two"] = 2;
    EXPECT_EQ(len(m), 2);

    m.remove("one");
    EXPECT_FALSE(m.has("one"));
    EXPECT_EQ(len(m), 1);
    EXPECT_EQ(m["two"], 2);
    EXPECT_EQ(len(m), 1);

    m.remove("two");
    EXPECT_EQ(len(m), 0);
}

TEST(Map, Dynamic) {
    var m = map_reserve<String<8>, int>(10);
    m["one"] = 1;
    m["two"] = 2;
    m["three"] = 3;
    m["three"] = 4;

    EXPECT_EQ(m["one"], 1);
    EXPECT_EQ(m["two"], 2);
    EXPECT_NE(m["three"], 3);
    EXPECT_EQ(m["three"], 4);
}

TEST(Map, util) {
    var m = Map<const char*, int>();
    m["one"] = 1;
    m["two"] = 2;
    EXPECT_EQ(m["one"], 1);
    EXPECT_EQ(m["two"], 2);

    var b = Vector({1, 2, 3, 4, 5, 6});
    var y = b | filter(lambda (val item) { return item % 2 == 0; });
    EXPECT_EQ(y(), 2);
    EXPECT_EQ(y(), 4);
    EXPECT_EQ(y(), 6);
}

