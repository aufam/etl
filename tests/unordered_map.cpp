#include "etl/unordered_map.h"
#include "etl/string.h"
#include "gtest/gtest.h"
#include "etl/placeholder.h"
#include "etl/keywords.h"

using namespace Project::etl;
using namespace Project::etl::literals;

TEST(unordered_map, Initialize) {
    var a = unordered_map<String<8>, int>(); // empty unordered_map
    EXPECT_EQ(a.begin(), null);
    EXPECT_EQ(a.len(), 0);

    val c = unordered_map(pair("one"s, 1), pair("two"s, 2), pair("three"s, 3)); // from variadic function
    EXPECT_NE(c.begin(), null);
    EXPECT_EQ(c.len(), 3);

    var d = unordered_map<String<8>, int>({{"one", 1}, {"two", 2}, {"three", 3}}); // from initializer_list
    EXPECT_NE(d.begin(), null);
    EXPECT_EQ(d.len(), 3);
}

TEST(unordered_map, Remove) {
    var m = unordered_map<String<8>, int>();
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

TEST(unordered_map, Dynamic) {
    var m = unordered_map<String<8>, int>();
    m["one"] = 1;
    m["two"] = 2;
    m["three"] = 3;
    m["three"] = 4;

    EXPECT_EQ(m["one"], 1);
    EXPECT_EQ(m["two"], 2);
    EXPECT_NE(m["three"], 3);
    EXPECT_EQ(m["three"], 4);
}

#if __cplusplus == 201703L
TEST(unordered_map, Util) {
    // unordered_map of name-age pairs
    val ages = unordered_map<String<8>, int>({
        {"Bob", 29},
        {"Mark", 23},
        {"Chuck", 23},
        {"Osas", 35},
        {"Jupri", 23}
    });

    // people who are 23
    var names = ages | 
        filter(placeholder::arg<1>.get<1>() == 23) |
        transform(placeholder::arg<1>.get<0>());

    EXPECT_EQ(next(names), "Mark");
    EXPECT_EQ(next(names), "Chuck");
    EXPECT_EQ(next(names), "Jupri");
}
#endif

