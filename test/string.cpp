#include "gtest/gtest.h"
#include "etl/string.h"

using namespace Project::etl;
using namespace Project::etl::literals;

TEST(String, Empty) {
    String s;
    EXPECT_EQ(s.len(), 0);
    EXPECT_EQ(s.rem(), s.size() - 1);
}

TEST(String, CStyleFormatter) {
    String s = {"Test %d", 123};
    EXPECT_EQ(s, "Test 123");
    EXPECT_EQ(s.len(), 8);
    EXPECT_EQ(s.rem(), s.size() - 1 - 8);

    s("pi ~= %d/%d", 22, 7);
    EXPECT_EQ(s, "pi ~= 22/7");
}

TEST(String, IterateThrough) {
    String s = {"Test %d", 123};
    s[5] = '4';
    s[6] = '5';
    s[7] = '6';
    EXPECT_TRUE(compare_all(s, "Test 456"));
}

TEST(String, Assign) {
    String s = "Test";
    EXPECT_EQ(s, "Test");
    s = {"Test %d", 123};
    EXPECT_EQ(s, "Test 123");
    s = string("123");
    EXPECT_EQ(s, "123");
    s = "abc";
    EXPECT_EQ(s, "abc");
    s = 'z';
    EXPECT_EQ(s, "z");
}

TEST(String, Append) {
    String<9> s = "Test";
    EXPECT_EQ(s, "Test");
    EXPECT_EQ(s.len(), 4);
    EXPECT_EQ(s.rem(), 4);

    s += ' ';
    EXPECT_EQ(s, "Test ");
    EXPECT_EQ(s.len(), 5);
    EXPECT_EQ(s.rem(), 3);

    s += string("123");
    EXPECT_EQ(s, "Test 123");
    EXPECT_EQ(s.len(), 8);
    EXPECT_EQ(s.rem(), 0);

    s += "\n"; // cannot, string buf is full
    EXPECT_EQ(s, "Test 123");
    EXPECT_EQ(s.len(), 8);
    EXPECT_EQ(s.rem(), 0);
}

TEST(String, IsContaining) {
    String f = {"Test %d%d%d", 1, 2, 3};
    EXPECT_TRUE(f.isContaining("123"));
    EXPECT_TRUE(!f.isContaining("321"));
}

TEST(String, SplitString) {
    auto ss = string("Test 123 abc").split();
    EXPECT_STREQ(ss[0], "Test");
    EXPECT_STREQ(ss[1], "123");
    EXPECT_STREQ(ss[2], "abc");
    EXPECT_EQ(ss[3], nullptr);
    EXPECT_EQ(ss[4], nullptr);
    EXPECT_EQ(len(ss), 3);

    auto s = string("Test 123");
    auto sp = SplitString(s);
    EXPECT_STREQ(sp[0], "Test");
    EXPECT_STREQ(sp[1], "123");
    EXPECT_EQ(len(sp), 2);
}

TEST(String, Constexpr) {
    // size is ETL_SHORT_STRING_DEFAULT_SIZE
    constexpr auto a = "abc"s;
    EXPECT_EQ(a, "abc");
    EXPECT_EQ(a.size(), ETL_SHORT_STRING_DEFAULT_SIZE);

    constexpr String<3> b = a;
    EXPECT_EQ(b, "ab");
    EXPECT_EQ(b.size(), 3);
    EXPECT_EQ(b.len(), 2);

    constexpr auto c = a[0];
    EXPECT_EQ(c, 'a');

    constexpr auto d = len(a); // or a.len()
    constexpr auto e = a.rem();
    constexpr auto f = a.front();
    constexpr auto g = a.back();

    // size is deduced
    constexpr auto s = string("Test ") + string("123 ") + string("abc");
    EXPECT_EQ(s, "Test 123 abc");
    EXPECT_EQ(s.size(), s.len() + 1);
}

TEST(String, Move) {
    auto a = "123456"s; // size is ETL_SHORT_STRING_DEFAULT_SIZE
    auto b = string("abc"); // size is 4
    EXPECT_EQ(b, "abc");
    b = move(a);
    EXPECT_EQ(b, "123");
}

TEST(String, Swap) {
    auto a = "123456"s; // size is ETL_SHORT_STRING_DEFAULT_SIZE
    auto b = string("abc"); // size is 4
    swap(a, b);
    EXPECT_EQ(a, "abc");
    EXPECT_EQ(b, "123");

    auto c = "123456"s; // size is ETL_SHORT_STRING_DEFAULT_SIZE
    auto d = string("abc"); // size is 4
    swap_element(c, d);
    EXPECT_EQ(c, "abc");
    EXPECT_EQ(d, "123");
}

TEST(String, Cast) {
    char a[] = "123456";
    auto& b = string_cast(a); // string size is automatically deduced
    auto& c = string_cast<8>(a + 1); // string size has to be explicitly specified
    EXPECT_EQ(b, "123456");
    EXPECT_EQ(c, "23456");
    EXPECT_EQ(&b[0], &a[0]);
    EXPECT_EQ(&c[0], &a[1]);

    const char d[] = "abcde";
    auto& e = string_cast(d); // string size is automatically deduced
    auto& f = string_cast<8>(d + 2); // string size has to be explicitly specified
    EXPECT_EQ(e, "abcde");
    EXPECT_EQ(&e[0], &d[0]);
    EXPECT_EQ(&f[0], &d[2]);
}