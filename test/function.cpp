#include "etl/function.h"
#include "etl/array.h"
#include "etl/linked_list.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project::etl;

TEST(Function, Context) {
    int i = 2;
    /// equivalent to:
    /// ```C++
    /// std::function<void()> iSquare = [&i]{ i *= i; };
    /// ```
    var iSquare = functionR(+lambda (int& i) { i *= i; }, i);
    iSquare();
    EXPECT_EQ(i, 4);
    EXPECT_TRUE(is_functor_v<decltype(iSquare)>);

    iSquare.fn = nullptr;
    iSquare(); // do nothing
    EXPECT_EQ(i, 4);
}

TEST(Function, NoContext) {
    /// equivalent to:
    /// ```C++
    /// std::function<int(int)> square = [](int i){ return i *= i; };
    /// ```
    val square = function<int(int)>(lambda (val i) { return i * i; });
    EXPECT_EQ(square(2), 4);
    EXPECT_TRUE(is_functor_v<decltype(square)>);
}

TEST(Function, ContextWithArgs) {
    int i = 2;
    /// equivalent to:
    /// ```C++
    /// std::function<void(int)> iMultiplyBy = [&i](int c){ i *= c; };
    /// ```
    val iMultiplyBy = Function<void(int), int&>(lambda (var i, val c) { i *= c; }, i);

    iMultiplyBy(10);
    EXPECT_EQ(i, 20);
    EXPECT_TRUE(is_functor_v<decltype(iMultiplyBy)>);
}

TEST(Function, Empty) {
    var f = function<int(int)>(); // empty function
    EXPECT_FALSE(f);

    f = lambda (int i) { return 2 * i; };
    EXPECT_TRUE(f);
    EXPECT_EQ(f(2), 4);

    int p = 10;
    f = function<int(int)>(lambda (int* ctx, int i) { *ctx += i; return *ctx; }, &p);
    EXPECT_EQ(f(2), p);
    EXPECT_EQ(p, 12);
}

TEST(Function, Compare) {
    int p = 0;
    val f = lambda (int n) { return n; };
    var g = function<int(int)>();
    var h = function<int(int)>(f);

    g = f;
    EXPECT_TRUE(g == f);
    EXPECT_TRUE(h == f);
    EXPECT_TRUE(g == h);
    EXPECT_TRUE(h == g);

    g = function<int(int)>(lambda (int* ctx, int n) { return *ctx * n; }, &p);
    h = nullptr;
    EXPECT_FALSE(g == h);
    EXPECT_FALSE(h == g);
}

TEST(Function, Array) {
    char text1[] = "test";
    char text2[] = "123";

    val arr = array(
        function<void()>(lambda (char* test) {
            std::cout << test << '\n';
        }, text1),
        function<void()>(lambda (char* test) {
            std::cout << test << '\n';
        }, text2)
    );

    for (var f in arr)
        f();
    
    EXPECT_EQ(arr[0].context, text1);
    EXPECT_EQ(arr[1].context, text2);
}

TEST(Function, LinkedList) {
    char text1[] = "test";
    char text2[] = "123";
    
    var l = list<Function<void(), void*>>();

    var f1 = function<void()>(lambda (char* test) {
        std::cout << test << '\n';
    }, text1);

    var f2 = function<void()>(lambda (char* test) {
        std::cout << test << '\n';
    }, text2);
    
    l << f1 << f2;

    for (var f in l)
        f();
    
    EXPECT_EQ(l[0], f1);
    EXPECT_EQ(l[1], f2);
}

TEST(Function, Bind) {
    class Process {
        int data;

    public:
        explicit Process(int data) : data(data) {}

        Function<int(), const Process*> getter = bind<&Process::get>(this);
        Function<void(int), Process*> setter = bind<&Process::set>(this);

    private:
        int get() const { return data; }
        void set(int d) { data = d; }
    };

    Process p(10);
    EXPECT_EQ(p.getter(), 10);
    p.setter(1);
    EXPECT_EQ(p.getter(), 1);
}