#include "etl/scope_exit.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

TEST(ScopeExit, ScopeExit) {
    int a = 10;
    {
        etl::ScopeExit scope_exit([&] { a *= 10; });
        a += 10;
    }
    EXPECT_EQ(a, 200);
}