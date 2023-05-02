#include "etl/map.h"
#include "etl/string.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project::etl;
using namespace Project::etl::literals;

TEST(Map, Initialize) {
    // var a = map<String<8>, int>(); // empty
    // EXPECT_EQ(a.begin(), null);
    // EXPECT_EQ(a.len(), 0);
    // EXPECT_EQ(a.size(), 0);

    // var b = map_reserve<String<8>, int>(10); // empty
    // EXPECT_NE(b.begin(), null);
    // EXPECT_EQ(b.len(), 0);
    // EXPECT_EQ(b.size(), 10);

    // var c = map(pair("one"s, 1), pair("two"s, 2), pair("three"s, 3));
    // EXPECT_NE(c.begin(), null);
    // EXPECT_EQ(c.len(), 3);
    // EXPECT_EQ(c.size(), 3);

    var d = map<String<8>, int>({{"one", 1}, {"two", 2}, {"three", 3}});
    EXPECT_NE(d.begin(), null);
    EXPECT_EQ(d.len(), 3);
    EXPECT_EQ(d.size(), 3);
}

// TEST(Map, Remove) {
//     var m = map<String<8>, int>();
//     m["one"] = 1;
//     m["two"] = 2;
//     EXPECT_EQ(len(m), 2);

//     m.remove("one");
//     EXPECT_FALSE(m.has("one"));
//     EXPECT_EQ(len(m), 1);
//     EXPECT_EQ(m["two"], 2);
//     EXPECT_EQ(len(m), 1);

//     m.remove("two");
//     EXPECT_EQ(len(m), 0);
// }

// TEST(Map, Dynamic) {
//     var m = map_reserve<String<8>, int>(10);
//     m["one"] = 1;
//     m["two"] = 2;
//     m["three"] = 3;
//     m["three"] = 4;

//     EXPECT_EQ(m["one"], 1);
//     EXPECT_EQ(m["two"], 2);
//     EXPECT_EQ(m["three"], 4);
// }

// TEST(Map, Deduced) {
//     val m = map<String<8>, int>({{"one", 1}, {"two", 2}, {"three", 3}});

//     EXPECT_EQ(m["one"], 1);
//     EXPECT_EQ(m["two"], 2);
//     EXPECT_EQ(m["three"], 3);
// }


