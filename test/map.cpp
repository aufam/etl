#include "etl/map.h"
#include "etl/string.h"
#include "gtest/gtest.h"

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