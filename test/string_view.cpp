#include "etl/string_view.h"
#include "gtest/gtest.h"
#include <string_view>

using namespace Project::etl;
using namespace Project::etl::literals;

TEST(StringView, Split) {
    auto split = " 123  456 "sv.split(" ");
    for (auto &sv : split) {
        std::cout << "string view test: " << std::string_view(sv.data(), sv.len()) << "end\n";
    }
    EXPECT_EQ(split[0], "123");
    EXPECT_EQ(split[1], "456");
    EXPECT_EQ(split[2], "");
    EXPECT_EQ(split[3], "");
}

TEST(StringView, Match) {
    auto match = "12 34 "sv.match("%s %s");
    for (auto &sv : match) {
        std::cout << "string view test: " << std::string_view(sv.data(), sv.len()) << "end\n";
    }
    EXPECT_EQ(match[0], "12");
    EXPECT_EQ(match[1], "34 ");
    EXPECT_EQ(match[2], "");
}

TEST(StringView, MatchAPI) {
    auto match = "/api"sv.match("/api/%s");
    for (auto &sv : match) {
        std::cout << "string view test: " << std::string_view(sv.data(), sv.len()) << "end\n";
    }
    EXPECT_EQ(match[0], "");
    EXPECT_EQ(match[1], "");
    EXPECT_EQ(match[2], "");
}

struct Request {
    StringView method, url, version, head, body;
    StringView get_head(StringView key) const;
    static Request parse(const uint8_t* buf, size_t len);
};

auto Request::get_head(StringView key) const -> StringView {
    auto pos = head.find(key);
    if (pos >= head.len())
        return nullptr;
    
    auto pos_end = pos + key.len();
    auto res = head.substr(pos_end, head.len() - pos_end);
    if (res.find(": ") == 0)
        res = res.substr(2, res.len() - 2);
    else if (res.find(":") == 0)
        res = res.substr(1, res.len() - 1);
    // else
        // bad request
    
    res = res.substr(0, res.find("\n"));
    if (res and res.back() == '\r')
        res = res.substr(0, res.len() - 1);

    return res; 
}

auto Request::parse(const uint8_t* buf, size_t len) -> Request {
    auto sv = string_view(buf, len);
    auto req = Request{};

    auto methods = sv.split<3>(" ");
    req.method = methods[0];
    req.url = methods[1];
    req.version = methods[2].split<1>("\n")[0];
    
    sv = req.version.end() + 1;
    if (req.version and req.version.back() == '\r')
        req.version = req.version.substr(0, req.version.len() - 1);
    
    auto head_end = sv.find("\r\n\r\n");
    auto body_start = head_end + 4;
    if (head_end == sv.len()) {
        head_end = sv.find("\n\n");
        body_start = head_end + 2;
    }
    // else bad request
    
    req.head = sv.substr(0, head_end);
    req.body = sv.substr(body_start, sv.len());

    auto body_len = len - (req.body.begin() - reinterpret_cast<const char*>(buf));
    req.body = string_view(req.body.begin(), body_len); 
    
    return req;
} 

static const char text[] = 
"POST /api/users HTTP/1.1\n"
"Content-Type: application/json\r\n"
"Authorization: Bearer your_access_token" 
"\n\n"
R"({
  "username": "john_doe",
  "email": "john.doe@example.com"
})";


TEST(StringView, Parse) {
    auto req = Request::parse(reinterpret_cast<const uint8_t*>(text), sizeof(text) - 1);

    std::cout << std::string_view(req.method.begin(), req.method.len()) << "...\n";
    std::cout << std::string_view(req.url.begin(), req.url.len()) << "...\n";
    std::cout << std::string_view(req.version.begin(), req.version.len()) << "...\n";
    std::cout << std::string_view(req.head.begin(), req.head.len()) << "...\n";
    std::cout << std::string_view(req.body.begin(), req.body.len()) << "...\n";
    auto ct = req.get_head("Content-Type");
    std::cout << std::string_view(ct.begin(), ct.len()) << "...\n";

    EXPECT_EQ(req.method, "POST");
    EXPECT_EQ(req.url, "/api/users");
    EXPECT_EQ(req.version, "HTTP/1.1");
    EXPECT_EQ(req.get_head("Content-Type"), "application/json");
    EXPECT_EQ(req.get_head("Content-Length"), "");
}