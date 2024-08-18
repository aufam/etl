#include "etl/json.h"
#include "etl/json_serialize.h"
#include "etl/json_deserialize.h"
#include "gtest/gtest.h"

using namespace Project::etl;
using namespace Project::etl::literals;

TEST(JSON, Parse) {
    constexpr auto json = Json::parse(R"(
        {
            "name": "John Doe",
            "age": 30,
            "isStudent": false,
            "grades": [90, 85, 92],
            "address": {
                "city": "New York",
                "zip": "10001"
            },
            "isActive": true,
            "info": null
        }
    )");

    EXPECT_EQ(json["name"].to_string(), "John Doe");
    EXPECT_EQ(json["age"].to_int(), 30);
    EXPECT_EQ(json["isStudent"].is_false(), true);
    EXPECT_EQ(json["grades"][0].to_int(), 90);
    EXPECT_EQ(json["grades"][1].to_int(), 85);
    EXPECT_EQ(json["grades"][2].to_int(), 92);
    EXPECT_EQ(json["address"]["city"].to_string(), "New York");
    EXPECT_EQ(json["address"]["zip"].to_string(), "10001");
    EXPECT_EQ(json["isActive"].is_true(), true);
    EXPECT_EQ(json["info"].is_null(), true);

    EXPECT_EQ(json.len(), 7);
}

TEST(JSON, Iterator) {
    auto json = Json::parse("[0, 1, 2, 3]");

    int i = 0;
    for (auto num : json) {
        EXPECT_EQ(num.to_int(), i);
        ++i;
    }
    EXPECT_EQ(i, 4);
    EXPECT_EQ(json.len(), 4);
}

TEST(JSON, StructuredBinding) {
    auto json = Json::parse(R"(
        {
            "generalManager": "Sugeng",
            "manager": "Bejo",
            "seniorStaff": "Prapto",
            "juniorStaff": "Derek"
        }
    )");

    StringView positions[4];
    StringView names[4];

    int i = 0;
    for (auto [position, name] : json) {
        positions[i] = position;
        names[i] = name.to_string();
        ++i;
    }

    EXPECT_EQ(positions[0], "generalManager");
    EXPECT_EQ(positions[1], "manager");
    EXPECT_EQ(positions[2], "seniorStaff");
    EXPECT_EQ(positions[3], "juniorStaff");

    EXPECT_EQ(names[0], "Sugeng");
    EXPECT_EQ(names[1], "Bejo");
    EXPECT_EQ(names[2], "Prapto");
    EXPECT_EQ(names[3], "Derek");

    EXPECT_EQ(json.len(), 4);
}

struct Bar {
    float num;
    bool is_true;
};

JSON_DEFINE(Bar, 
    JSON_ITEM("num", num), 
    JSON_ITEM("is_true", is_true)
)
struct Foo {
    int num;
    std::string text;
    Bar bar;
};

JSON_DEFINE(Foo, 
    JSON_ITEM("num", num), 
    JSON_ITEM("text", text),
    JSON_ITEM("bar", bar)
)

TEST(JSON, Serialize) {
    json::List_<json::Map> l {
        12345, 
        3.14, 
        "test"sv,
        json::Map {
            {"num", 42},
            {"dynamic_string", std::string("dynamic")},
        },
    };

    Foo foo {
        .num = 42,
        .text = "test",
        .bar = {
            .num = 3.14,
            .is_true = false,
        }
    };

    EXPECT_EQ(json::serialize(l), "[12345,3.140,\"test\",{\"num\":42,\"dynamic_string\":\"dynamic\"}]");
    EXPECT_EQ(json::serialize(foo), "{\"num\":42,\"text\":\"test\",\"bar\":{\"num\":3.14,\"is_true\":false}}");
}

TEST(JSON, Deserialize) {
    auto foo = json::deserialize<Foo>(R"({
        "num": 24, 
        "text": "test",
        "bar": {
            "num": 3.14, 
            "is_true": true
        }
    })").unwrap();

    EXPECT_EQ(foo.num, 24);
    EXPECT_EQ(foo.text, "test");
    EXPECT_EQ(foo.bar.num, 3.14f);
    EXPECT_EQ(foo.bar.is_true, true);

    const auto bar = json::deserialize<Bar>(R"({
        "num": 3.14, 
        "is_true": false
    })").unwrap();

    EXPECT_EQ(bar.num, 3.14f);
    EXPECT_EQ(bar.is_true, false);
}

TEST(JSON, Float) {
    auto a = json::size_max(0.1f);
    auto b = json::size_max(1.12343f);
    auto c = json::size_max(100.0f);
    auto d = json::size_max(-3.14f);

    EXPECT_EQ(a, 4);
    EXPECT_EQ(b, 4);
    EXPECT_EQ(c, 6);
    EXPECT_EQ(d, 5);

    EXPECT_EQ(json::serialize(0.1f), "0.10");
    EXPECT_EQ(json::serialize(1.3433f), "1.34");
    EXPECT_EQ(json::serialize(100.0f), "100.00");
    EXPECT_EQ(json::serialize(-3.14f), "-3.14");
}