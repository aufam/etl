#include "etl/any.h"
#include "etl/string.h"
#include "etl/vector.h"
#include "etl/linked_list.h"
#include "gtest/gtest.h"

#include "etl/keywords.h"

using namespace Project::etl;

TEST(Any, test) {
    struct Message {
        uint8_t cmd;
        union { float f; int i; uint32_t u; };
    };

    uint8_t buffer[sizeof(Message)];
    auto ptr = new (buffer) Message{.cmd=10, .u=2};
    std::cout << "cmd: " << int(ptr->cmd) << '\n'
              << "u: " << ptr->u << '\n'
              << "i: " << ptr->i << '\n'
              << "f: " << ptr->f << '\n';
}

TEST(Any, Dynamic) {
    Any a = 10;
    EXPECT_EQ(a.as<int>(), 10);

    a.as<int>()++;
    EXPECT_EQ(a.as<int>(), 11);

    using ss = String<16>;
    a = ss("Test 123");
    EXPECT_EQ(a.size(), ss::size());
#if __cplusplus == 201703L
    EXPECT_EQ(a.as<ss>(), "Test 123");
#endif

    Any b = a;
    EXPECT_EQ(b.size(), ss::size());
#if __cplusplus == 201703L
    EXPECT_EQ(a.as<ss>(), "Test 123");
    EXPECT_EQ(b.as<ss>(), "Test 123");
#endif

    Any c = move(a);
    EXPECT_FALSE(a);
#if __cplusplus == 201703L
    EXPECT_EQ(b.as<ss>(), "Test 123");
    EXPECT_EQ(c.as<ss>(), "Test 123");
#endif

    c = "Test 123";
    EXPECT_EQ(c.as<const char*>(), "Test 123");

    // assigning a type that has custom destructor
    a = vector(1, 2, 3);
    EXPECT_EQ(a.as<Vector<int>>()[0], 1);
    EXPECT_EQ(a.as<Vector<int>>()[1], 2);
    EXPECT_EQ(a.as<Vector<int>>()[2], 3);
}

TEST(Any, Vector) {
    var v = vector<Any<>>();

    Any a = 10;
    v += move(a);

    Any b = 20;
    v += b;

    EXPECT_FALSE(a);
    EXPECT_TRUE(b);

    EXPECT_EQ(v[0].as<int>(), 10);
    EXPECT_EQ(v[1].as<int>(), 20);

    v = vector<Any<>>(string<8>("test"), move(b));

    EXPECT_FALSE(b);
#if __cplusplus == 201703L
    EXPECT_EQ(v[0].as<String<8>>(), "test");
#endif
    EXPECT_EQ(v[1].as<int>(), 20);
}

TEST(Any, LinkedList) {
    var l = list<Any<>>();

    Any a = 10;
    Any b = 20;

    l << move(a) << b;

    EXPECT_FALSE(a);
    EXPECT_TRUE(b);

    EXPECT_EQ(l[0].as<int>(), 10);
    EXPECT_EQ(l[1].as<int>(), 20);

    l = list<Any<>>(string<8>("test"), move(b));

    EXPECT_FALSE(b);
#if __cplusplus == 201703L
    EXPECT_EQ(l[0].as<String<8>>(), "test");
#endif
    EXPECT_EQ(l[1].as<int>(), 20);
}
