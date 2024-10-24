#include "etl/async.h"
#include <thread>
#include <chrono>
#include "gtest/gtest.h"
#include "etl/keywords.h"


using namespace std::literals;

static const etl::Async<int(int)> example = [] (int num) {
    std::this_thread::sleep_for(100ms);
    return num + num;
};

TEST(Async, Launch) {
    // launch in another thread
    auto res = example(10).then([] (int num) {
        return num * 10;
    }).launch();

    EXPECT_EQ(res.get(), 200);
}

TEST(Async, Await) {
    // invoke in this thread
    auto res = example(10).then([] (int num) {
        return num * 10;
    }).await();

    EXPECT_EQ(res, 200);
}