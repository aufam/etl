#include "etl/memory.h"
#include "etl/string.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project::etl;

#if __cplusplus == 201703L
TEST(Memory, Unique) {
    var a = make_unique<String<16>>("test %d", 123);
    var b = move(a);
    EXPECT_FALSE(a);
    EXPECT_EQ(a.get_value_or("moved"), "moved");
    EXPECT_EQ(*b, "test 123");

    var c = b.release();
    EXPECT_FALSE(b);
    EXPECT_EQ(b.get_value_or("released"), "released");
    EXPECT_EQ(*c, "test 123");

    var d = unique_ptr<String<16>>(c);
}
#endif

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

#if __cplusplus == 201703L
        EXPECT_EQ(*b, "test 123");
        EXPECT_EQ(*c, "test 123");
        EXPECT_EQ(*d, "test 123");
#endif

        *d = "modified";
    }

    EXPECT_EQ(a.count(), 0);
    EXPECT_EQ(b.count(), 2);
    EXPECT_EQ(c.count(), 2);

    EXPECT_FALSE(a);
#if __cplusplus == 201703L
    EXPECT_EQ(*b, "modified");
    EXPECT_EQ(*c, "modified");
#endif

    var [d, cnt] = b.release();
    EXPECT_FALSE(b);

    EXPECT_EQ(a.count(), 0);
    EXPECT_EQ(b.count(), 0);
    EXPECT_EQ(c.count(), 1);
    EXPECT_EQ(cnt, 1);

#if __cplusplus == 201703L
    EXPECT_EQ(b.get_value_or("released"), "released");
    EXPECT_EQ(*c, "modified");
    EXPECT_EQ(*d, "modified");
#endif

    // properly handle d
    if (cnt == 0) delete d;
}