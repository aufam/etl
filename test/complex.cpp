#include "gtest/gtest.h"
#include "etl/complex.h"

using namespace Project::etl;
using namespace Project::etl::literals;

TEST(Complex, Conversion) {
    /* from complex8 */ {
        const auto a = complex8{1, -1}; {
            const auto b = static_cast<complex16> (a);
            const auto c = complex16{1 * 0x1'00, -1 * 0x1'00};
            EXPECT_EQ(b.real, c.real);
            EXPECT_EQ(b.imag, c.imag);
            EXPECT_TRUE(a == b and a == c);
        } {
            const auto b = static_cast<complex32> (a);
            const auto c = complex32{1 * 0x100'0000, -1 * 0x100'0000};
            EXPECT_EQ(b.real, c.real);
            EXPECT_EQ(b.imag, c.imag);
            EXPECT_TRUE(a == b and a == c);
        }
    } /* from complex32 */ {
        const auto a = complex32{0x1'0000, -0x1'0000}; {
            const auto b = static_cast<complex8> (a);
            const auto c = complex8{};
            EXPECT_EQ(b.real, c.real);
            EXPECT_EQ(b.imag, c.imag);
            EXPECT_TRUE(a == b and a == c);
        } {
            const auto b = static_cast<complex16> (a);
            const auto c = complex16{1, -1};
            EXPECT_EQ(b.real, c.real);
            EXPECT_EQ(b.imag, c.imag);
            EXPECT_TRUE(a == b and a == c);
        }
    }
}

TEST(Complex, Arithmetic) {
    /* operator +- */ {
        const auto a = complex8 { 1, -1 };
        const auto b = complex16 { 0x1'00, -1}; {
            const auto c = a + b;
            EXPECT_EQ(c.real, 2);
            EXPECT_EQ(c.imag, -1); // b.imag is too small compared to a.imag
        } {
            const auto c = a - b;
            EXPECT_EQ(c.real, 0);
            EXPECT_EQ(c.imag, -1); // b.imag is too small compared to a.imag
        } {
            const auto c = a + (0.1 + 0.1_i);
            EXPECT_EQ(c.real, a.real + static_cast<complex8::value_type> (0.1 * complex8::max_value_f));
            EXPECT_EQ(c.imag, a.imag + static_cast<complex8::value_type> (0.1 * complex8::max_value_f));
        } {
            const auto c = a + 0.1;
            EXPECT_EQ(c.real, a.real + static_cast<complex8::value_type> (0.1 * complex8::max_value_f));
            EXPECT_EQ(c.imag, a.imag);
        } {
            const auto c = 0.1 + a;
            EXPECT_EQ(c.real, a.real + static_cast<complex8::value_type> (0.1 * complex8::max_value_f));
            EXPECT_EQ(c.imag, a.imag);
        }  /* not allowed
            const auto c = a + 2;
            const auto c = a - 3;
        */
    } /* operator *\ */ {
        const auto a = complex8 { 10, -10 };
        const auto b = complex16 { 0x1'00, -1}; {
            const auto c = a * 2;
            EXPECT_EQ(c.real, 20);
            EXPECT_EQ(c.imag, -20);
        } {
            const auto c = 2 * a;
            EXPECT_EQ(c.real, 20);
            EXPECT_EQ(c.imag, -20);
        } {
            const auto d = 1.0_i;
            const auto c = a * d;
            EXPECT_EQ(c.real, 10);
            EXPECT_EQ(c.imag, 10);
        } {
            const auto c = a / 2;
            EXPECT_EQ(c.real, 5);
            EXPECT_EQ(c.imag, -5);
        }
    }
}

TEST(Complex, Compare) {
    const auto a = complex8  { 0x7f, 0x7f };
    const auto b = complex16 { 0x7fff, 0x7fff };
    const auto c = complex32 { 0x7fff'ffff, 0x7fff'ffff };
    const auto d = 1.0 + 1.0_i;
    EXPECT_TRUE(a == b and a == c and a == d and b == c and b == d and c == d);

    const auto e = static_cast<complex8> (c);
    const auto f = e.real;
    const auto g = e.imag;

    const auto h = a.magnitude_square();
    const auto i = e.magnitude_square();
    const auto j = d.magnitude_square();

}