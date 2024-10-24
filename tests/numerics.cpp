#include "etl/numerics.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project::etl;

template <typename T, typename U>
struct common_type_helper {
    using type = remove_reference_t<decltype(true ? declval<T>() : declval<U>())>;
};

using t = std::common_type<int, unsigned int>::type;

TEST(Numeric, Arithmetics) {
    EXPECT_TRUE(safe_lt(-1, 10u));
    EXPECT_TRUE(safe_le(-1, 10.f));
    EXPECT_TRUE(safe_gt(2.0, 1));
    EXPECT_TRUE(safe_ge(char(100), -1));
    EXPECT_TRUE(safe_eq(char(100), 100));
    EXPECT_TRUE(safe_ne(char(100), -100));

    EXPECT_EQ(safe_add(-100, (unsigned char)(10)), -90);
    EXPECT_EQ(safe_sub(10u, 100u), -90);
    EXPECT_EQ(safe_mul(10u, -100), -1000);

    EXPECT_FLOAT_EQ(safe_mod(17.5, 4), 1.5);
    EXPECT_EQ(safe_mod<int>(17.01, 4), 1);
    EXPECT_FLOAT_EQ(safe_mod(17.5, 3.5), 0.0);


    EXPECT_EQ(safe_sub<unsigned int>(100u, -10), 110);

    EXPECT_EQ(safe_add<uint32_t>(-10, (unsigned char)(100)), 90);

    EXPECT_EQ(safe_mul<uint32_t>(-10, -1), 10);

    EXPECT_FLOAT_EQ(safe_truediv(1, 5), 0.2);

}

TEST(Numeric, Modulo) {
    // arg: int, return: int
    EXPECT_EQ(safe_mod(10, 3), 1);
    EXPECT_EQ(safe_mod(-10, 3), -1);
    EXPECT_EQ(safe_mod(10, -3), 1);
    EXPECT_EQ(safe_mod(-10, -3), -1);
    EXPECT_EQ(safe_mod(10, 0), 10);
    EXPECT_EQ(safe_mod(-10, 0), -10);

    // arg float, return: int
    EXPECT_EQ(safe_mod<int>(14.34 * 100, 100), 34);
    EXPECT_EQ(safe_mod<int>(-14.34 * 100, 100), -34);
    EXPECT_EQ(safe_mod<int>(14.34 * 100, -100), 34);
    EXPECT_EQ(safe_mod<int>(-14.34 * 100, -100), -34);
    EXPECT_EQ(safe_mod<int>(14.34, 0), 14);
    EXPECT_EQ(safe_mod<int>(-14.34, 0), -14);

    // arg float, return: float
    EXPECT_DOUBLE_EQ(safe_mod(15.5, 3.5), 1.5);
    EXPECT_DOUBLE_EQ(safe_mod(-15.5, 3.5), -1.5);
    EXPECT_DOUBLE_EQ(safe_mod(15.5, -3.5), 1.5);
    EXPECT_DOUBLE_EQ(safe_mod(-15.5, -3.5), -1.5);
    EXPECT_TRUE(std::isnan(safe_mod(15.5, 0)));
    EXPECT_TRUE(std::isnan(safe_mod(-15.5, 0)));

    // arg float, return: float
    {   
        auto [div, mod] = safe_divmod(17.5, 4);
        EXPECT_DOUBLE_EQ(div, 4.0);
        EXPECT_DOUBLE_EQ(mod, 1.5);
    } {   
        auto [div, mod] = safe_divmod(-17.5, 4);
        EXPECT_DOUBLE_EQ(div, -4.0);
        EXPECT_DOUBLE_EQ(mod, 1.5);
    } {   
        auto [div, mod] = safe_divmod(17.5, -4);
        EXPECT_DOUBLE_EQ(div, -4.0);
        EXPECT_DOUBLE_EQ(mod, 1.5);
    } {   
        auto [div, mod] = safe_divmod(-17.5, -4);
        EXPECT_DOUBLE_EQ(div, 4.0);
        EXPECT_DOUBLE_EQ(mod, 1.5);
    } 
    
    // arg float, return: int
    {
        auto [div, mod] = safe_divmod<int>(17.34 * 100, 100);
        EXPECT_EQ(div, 17);
        EXPECT_EQ(mod, 34);
    } {
        auto [div, mod] = safe_divmod<int>(-17.34 * 100, 100);
        EXPECT_EQ(div, -17);
        EXPECT_EQ(mod, 34);
    } {
        auto [div, mod] = safe_divmod<int>(17.34 * 100, -100);
        EXPECT_EQ(div, -17);
        EXPECT_EQ(mod, 34);
    } {
        auto [div, mod] = safe_divmod<int>(-17.34 * 100, -100);
        EXPECT_EQ(div, 17);
        EXPECT_EQ(mod, 34);
    } 
    
    // arg int, return: int
    {
        auto [div, mod] = safe_divmod(1734, 100);
        EXPECT_EQ(div, 17);
        EXPECT_EQ(mod, 34);
    } {
        auto [div, mod] = safe_divmod(-1734, 100);
        EXPECT_EQ(div, -17);
        EXPECT_EQ(mod, 34);
    } {
        auto [div, mod] = safe_divmod(1734, -100);
        EXPECT_EQ(div, -17);
        EXPECT_EQ(mod, 34);
    } {
        auto [div, mod] = safe_divmod(-1734, -100);
        EXPECT_EQ(div, 17);
        EXPECT_EQ(mod, 34);
    } 
}