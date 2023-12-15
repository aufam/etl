#include "etl/json.h"
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
}

TEST(JSON, Iterator) {
    auto json = Json::parse("[0, 1, 2, 3]");

    int i = 0;
    for (auto num : json) {
        EXPECT_EQ(num.to_int(), i);
        ++i;
    }
    EXPECT_EQ(i, 4);
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
}