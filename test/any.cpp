#include "etl/any.h"
#include "etl/string.h"
#include "gtest/gtest.h"

#include "etl/keywords.h"

using namespace Project::etl;

TEST(Any, Dynamic) {
    Any a;
    a = 10;
    EXPECT_EQ(a.as<int>(), 10);

    a.as<int>()++;
    EXPECT_EQ(a.as<int>(), 11);

    using ss = String<16>;
    a = ss("Test 123");
    EXPECT_EQ(a.as<ss>(), "Test 123");

    Any b = a;
    EXPECT_EQ(b.size(), ss::size());

    Any c;
    c = a;
    EXPECT_EQ(a.as<ss>(), "Test 123");
}