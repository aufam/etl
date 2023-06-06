#include "etl/ref.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project::etl;

TEST(Ref, Ref) {
    struct Struct { int a, b; };

    val c = Struct { 12, 13 };

    val a = ref<Struct>();
    val b = ref_const(c);
    val s = a.get_value_or(Struct{10, 11});

    EXPECT_EQ(s.a, 10);
    EXPECT_EQ(s.b, 11);
    EXPECT_EQ(b->a, 12);
    EXPECT_EQ(b->b, 13);
}
