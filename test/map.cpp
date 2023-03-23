#include "gtest/gtest.h"
#include "etl/map.h"
#include "etl/string.h"
#include "etl/keywords.h"

using namespace Project::etl;

TEST(Map, Dynamic) {
    var m = map<String<8>, int>();
    m["one"] = 1;
    m["two"] = 2;
    m["three"] = 3;
    m["three"] = 4;

    EXPECT_EQ(m["one"], 1);
    EXPECT_EQ(m["two"], 2);
    EXPECT_EQ(m["three"], 4);
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

TEST(Map, Deduced) {
    val m = map(pair("one", 1), pair("two", 2), pair("three", 3));

    EXPECT_EQ(m["one"], 1);
    EXPECT_EQ(m["two"], 2);
    EXPECT_EQ(m["three"], 3);
}
