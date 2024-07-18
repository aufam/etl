#include "gtest/gtest.h"
#include "etl/utility_basic.h"
#include "etl/keywords.h"

#define defer auto __defer__ = ::Project::etl::defer |

TEST(ScopeExit, ScopeExit) {
    int a = 10;
    {
        defer [&]() { a *= 10; };
        a += 10;
    }
    EXPECT_EQ(a, 200);
}
