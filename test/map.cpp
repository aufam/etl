#include "gtest/gtest.h"
#include "etl/map.h"
#include "etl/string.h"

using namespace Project::etl;

TEST(Map, dynamic) {
    Map<String<8>, int> map;
    map["one"] = 1;
    map["two"] = 2;
    map["three"] = 3;
    map["three"] = 4;

    EXPECT_EQ(map["one"], 1);
    EXPECT_EQ(map["two"], 2);
    EXPECT_EQ(map["three"], 4);
}

TEST(Map, remove) {
    Map<String<8>, int> map;
    map["one"] = 1;
    map["two"] = 2;
    EXPECT_EQ(len(map), 2);

    map.remove("one");
    EXPECT_FALSE(map.has("one"));
    EXPECT_EQ(len(map), 1);
    EXPECT_EQ(map["two"], 2);
    EXPECT_EQ(len(map), 1);

    map.remove("two");
    EXPECT_EQ(len(map), 0);
}