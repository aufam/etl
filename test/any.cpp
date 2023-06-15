#include "etl/any.h"
#include "etl/string.h"
#include "etl/vector.h"
#include "gtest/gtest.h"

#include "etl/keywords.h"

using namespace Project::etl;

TEST(Any, Dynamic) {
    Any a = 10;
    EXPECT_EQ(a.as<int>(), 10);

    a.as<int>()++;
    EXPECT_EQ(a.as<int>(), 11);

    using ss = String<16>;
    a = ss("Test 123");
    EXPECT_EQ(a.size(), ss::size());
    EXPECT_EQ(a.as<ss>(), "Test 123");

    Any b = a;
    EXPECT_EQ(b.size(), ss::size());
    EXPECT_EQ(a.as<ss>(), "Test 123");
    EXPECT_EQ(b.as<ss>(), "Test 123");

    Any c = move(a);
    EXPECT_FALSE(a);
    EXPECT_EQ(b.as<ss>(), "Test 123");
    EXPECT_EQ(c.as<ss>(), "Test 123");

    c = "Test 123";
    EXPECT_EQ(c.as<const char*>(), "Test 123");

    // assigning a type that has custom destructor
    a = vector(1, 2, 3);
    EXPECT_EQ(a.as<Vector<int>>()[0], 1);
    EXPECT_EQ(a.as<Vector<int>>()[1], 2);
    EXPECT_EQ(a.as<Vector<int>>()[2], 3);

    // properly delete object
    a.detach<Vector<int>>();
}