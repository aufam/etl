#include "etl/result.h"
#include "etl/placeholder.h"
#include "gtest/gtest.h"
#include <variant>
#include "etl/keywords.h"

using namespace etl;
using namespace etl::placeholder;

TEST(Result, Example) {
    {
        Result<int, std::string> a = Ok(50);
        Result<int, std::string> b = Err("Error");

        EXPECT_EQ(a.then(_1 + 2).unwrap(), 52);
        EXPECT_EQ((a + 2).unwrap(), 52);
        EXPECT_EQ(b.except(_1 + "123").unwrap_err(), "Error123");
    }

    {
        Result<int, std::string> a = Ok(50);
        Result<int, std::string> b = Err("Error");

        var [x, __] = etl::move(a);
        var [___, y] = b;

        EXPECT_TRUE(x);
        EXPECT_TRUE(y);

        EXPECT_EQ(*x, 50);
        EXPECT_EQ(*y, "Error");

        std::cout << "b after deref: " << b.unwrap_err() << "\n";
    }
}

TEST(Result, Void) {
    Result<void, std::string> a = Ok();
    Result<void, std::string> b = Err("Error");

    var [x, __] = a;
    var [___, y] = b;

    EXPECT_TRUE((etl::is_same_v<decltype(x), bool>));
    EXPECT_EQ(*y, "Error");
}

TEST(Result, Move) {
    class [[nodiscard]] LockGuard {
        int id;
    public:
        LockGuard(int id) : id(id) { printf("Lock\n"); }
        LockGuard(LockGuard&& other) : id(etl::exchange(other.id, 0)) {}
        ~LockGuard() { if (id) printf("Unlock\n"); }
    };

    auto f = [] () -> Result<LockGuard, int> {
        return Ok(LockGuard(10));
    };

    std::function<Result<LockGuard, int>()> fn = f;

    {
        auto res = fn();
        printf("Some work\n");
    }
}

TEST(Result, Expect) {
    auto division = [](int a, int b) -> Result<int, bool> {
        if (b == 0) {
            return Err(true);
        } else {
            return Ok(a / b);
        }
    };

    bool is_err = false;
    auto a = division(10, 2).expect([&](bool) { is_err = true; });
    EXPECT_EQ(a, 5);
    EXPECT_FALSE(is_err);

    EXPECT_THROW({
        auto b [[maybe_unused]] = division(10, 0).expect([&](bool err) { is_err = err; });
    }, std::bad_variant_access);
    EXPECT_TRUE(is_err);
}
