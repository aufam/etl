#include "gtest/gtest.h"
#include "etl/bit.h"
#include "etl/math.h"

using namespace Project::etl;

TEST(Bit, Bit_Cast) {
    EXPECT_EQ(bit_cast<uint32_t>(+0.0f), 0u);
    EXPECT_EQ(bit_cast<uint32_t>(-0.0f), 1u << 31);
    EXPECT_EQ(bit_cast<uint32_t>(INFINITY), 0xFFu << 23);
    EXPECT_EQ(bit_cast<uint32_t>(-INFINITY), 1u << 31 | 0xFFu << 23);
    EXPECT_EQ(bit_cast<uint32_t>(NAN), 0xFFu << 23 | 1u << 22);
    EXPECT_EQ(bit_cast<uint32_t>(-NAN), 1u << 31 | 0xFFu << 23 | 1u << 22);

    union {
        float f;
        uint8_t u[4];
    };
    f = 10.f;
    EXPECT_EQ(bit_cast<float>(u), f);
}