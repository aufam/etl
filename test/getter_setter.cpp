#include "etl/getter_setter.h"
#include "etl/placeholder.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace Project;
using namespace Project::etl;


TEST(GetterSetter, Get) {
    float value = 10;
    val g = make_getter<float>([&value] { return value; });

    float get_value = int(g);
    const bool l = is_convertible_v<float, float>;
    EXPECT_EQ(get_value, 10);
    EXPECT_EQ(get_value, value);
}

TEST(GetterSetter, Set) {
    float value = 10;
    val g = make_setter<float>([&value] (float new_value) { value = new_value; });

    g = 20;
    EXPECT_EQ(value, 20);
}

TEST(GetterSetter, GetSet) {
    float value = 10;
    val g = make_getter_setter<float>([&value] { return value; }, [&value] (float new_value) { value = new_value; });

    g = 20;
    val get_value = float(g);
    EXPECT_EQ(get_value, 20);
    EXPECT_EQ(get_value, value);
}

TEST(GetterSetter, Class) {
    struct Data {
        int i; float f; bool b;
    };

    class Process {
        Data& data;

    public:
        Process(Data& data) : data(data) {}

        GetterSetter<int, std::function<int()>, std::function<void(int)>> i = {
            [this] { return data.i; },
            [this] (int value) { data.i = value; }
        }; 

        Getter<float, std::function<float()>> f = {
            [this] { return data.f; }
        }; 
        
        Setter<bool, std::function<void(bool)>> b = {
            [this] (bool value) { data.b = value; }
        }; 
    };

    Data data { 1, 0.1, false }; // data is private
    Process process { data }; // data getter and setter

    process.b = true; // more readable than `data.set_b(true)`
    int i_now = process.i; // more readable than `int i_now = data.get_i()`

    EXPECT_EQ(process.i, 1);
}
