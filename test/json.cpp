#include "etl/json.h"
#include "gtest/gtest.h"

using namespace Project::etl;
using namespace Project::etl::literals;

TEST(JSON, Parse) {
    auto json = Json(R"(
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