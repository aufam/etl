#include "etl/complex.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project::etl;
using namespace Project::etl::literals;

TEST(Complex, Conversion) {
    /* from complex8 */ {
        val a = complex8(1, -1); {
            val b = static_cast<complex16>(a);
            val c = complex16(0x1'00, -0x1'00);
            EXPECT_TRUE(a == b);
            EXPECT_TRUE(a == c);
        } {
            val b = static_cast<complex32> (a);
            val c = complex32(0x100'0000, -0x100'0000);
            EXPECT_TRUE(a == b);
            EXPECT_TRUE(a == c);
        }
    } /* from complex32 */ {
        val a = complex32{0x1'0000, -0x1'0000}; {
            val b = static_cast<complex8>(a);
            val c = complex8(); // too small, so it is 0 + 0i
            EXPECT_TRUE(a == b);
            EXPECT_TRUE(a == c);
        } {
            val b = static_cast<complex16>(a);
            val c = complex16(1, -1);
            EXPECT_TRUE(a == b);
            EXPECT_TRUE(a == c);
        }
    }
}

TEST(Complex, Arithmetic) {
    /* operator +- */ {
        val a = complex8(1, -1);
        val b = complex16(0x1'00, -1); {
            val c = a + b;
            EXPECT_EQ(c.real, 2);
            EXPECT_EQ(c.imag, -1); // b.imag is too small compared to a.imag
        } {
            val c = a - b;
            EXPECT_EQ(c.real, 0);
            EXPECT_EQ(c.imag, -1); // b.imag is too small compared to a.imag
        } {
            val c = a + (0.1 + 0.1_i);
            EXPECT_EQ(c.real, a.real + static_cast<complex8::value_type> (0.1 * complex8::max_value_f));
            EXPECT_EQ(c.imag, a.imag + static_cast<complex8::value_type> (0.1 * complex8::max_value_f));
        } {
            val c = a + 0.1;
            EXPECT_EQ(c.real, a.real + static_cast<complex8::value_type> (0.1 * complex8::max_value_f));
            EXPECT_EQ(c.imag, a.imag);
        } {
            val c = 0.1 + a;
            EXPECT_EQ(c.real, a.real + static_cast<complex8::value_type> (0.1 * complex8::max_value_f));
            EXPECT_EQ(c.imag, a.imag);
        }  /* not allowed
            val c = a + 2;
            val c = a - 3;
        */
    } /* operator *\ */ {
        val a = complex8(10, -10); {
            val b = a * 2;
            val c = complex8(20, -20);
            EXPECT_EQ(b, c);
        } {
            val b = 2 * a;
            val c = complex8(20, -20);
            EXPECT_EQ(b, c);
        } {
            val d = 1.0_i;
            val b = a * d;
            val c = complex8(-10, 10);
            EXPECT_EQ(b, c);
        } {
            val b = a / 2;
            val c = complex8(5, -5);
            EXPECT_EQ(b, c);
        } {
            val b = 0.1 + 0.3_i;
            val c = 0.1 - 0.2_i;
            val d = b * c;
            val e = 0.07 + 0.01_i;
            val f = d.to_float();
            val g = e.to_float();
            EXPECT_EQ(d, e);
            EXPECT_NEAR(f.x, g.x, 0.01);
            EXPECT_NEAR(f.y, g.y, 0.01);
        }
    }
}

TEST(Complex, Compare) {
    val a = complex8 (0x7f, 0x7f);
    val b = complex16(0x7f00, 0x7f00);
    val c = complex32(0x7f00'0000, 0x7f00'0000);
    val d = 1.0 + 1.0_i;
    EXPECT_TRUE(a == b);
    EXPECT_TRUE(a == c);
    EXPECT_TRUE(a == d);
    EXPECT_TRUE(b == c);
    EXPECT_TRUE(b == d);
    EXPECT_TRUE(c == d);

    val e = static_cast<complex8>(c);
    val f = e.real;
    val g = e.imag;

    val h = a.magnitude_square();
    val i = e.magnitude_square();
    val j = d.magnitude_square();
}
