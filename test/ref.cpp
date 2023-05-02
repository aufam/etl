#include "etl/ref.h"
#include "gtest/gtest.h"

using namespace Project::etl;

TEST(Ref, Ref) {
    struct Struct {
        int a, b;
        constexpr explicit Struct(int a = 10, int b = 11) : a(a), b(b) {}
    };
    constexpr ConstRef<Struct> s = nullptr;
    constexpr ConstRef<Struct> d = Struct{12, 13};

    EXPECT_EQ(s->a, 10);
    EXPECT_EQ(s->b, 11);
    EXPECT_EQ(d->a, 12);
    EXPECT_EQ(d->b, 13);
}
