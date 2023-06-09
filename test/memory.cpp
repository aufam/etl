#include "etl/memory.h"
#include "etl/string.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project::etl;

TEST(Memory, Unique) {
    var a = make_unique<String<16>>("test %d", 123);
    var b = move(a);
    EXPECT_FALSE(a);
    EXPECT_EQ(a.get_value_or("moved"), "moved");
    EXPECT_EQ(*b, "test 123");
}

TEST(Memory, Shared) {
    var a = make_shared<String<16>>("test %d", 123);
    EXPECT_EQ(a.count(), 1);

    var b = a;
    EXPECT_EQ(a.count(), 2);
    EXPECT_EQ(b.count(), 2);

    var c = move(a);
    EXPECT_EQ(a.count(), 0);
    EXPECT_EQ(b.count(), 2);
    EXPECT_EQ(c.count(), 2);

    {
        var d = c;
        EXPECT_EQ(a.count(), 0);
        EXPECT_EQ(b.count(), 3);
        EXPECT_EQ(c.count(), 3);
        EXPECT_EQ(d.count(), 3);

        EXPECT_EQ(*b, "test 123");
        EXPECT_EQ(*c, "test 123");
        EXPECT_EQ(*d, "test 123");

        *d = "modified";
    }

    EXPECT_EQ(a.count(), 0);
    EXPECT_EQ(b.count(), 2);
    EXPECT_EQ(c.count(), 2);

    EXPECT_FALSE(a);
    EXPECT_EQ(*b, "modified");
    EXPECT_EQ(*c, "modified");
}