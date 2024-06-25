#ifndef ETL_JSON_TYPES_H
#define ETL_JSON_TYPES_H

#include "etl/string.h"
#include "etl/optional.h"
#include "etl/vector.h"
#include "etl/linked_list.h"
#include "etl/map.h"
#include "etl/unordered_map.h"
#include "etl/getter_setter.h"
#include "etl/ref.h"
#include <string>
#include <variant>
#include <cmath>
#include <tuple>


namespace Project::etl::json {
    #define JSON_BASIC_TYPES std::nullptr_t, bool, int, double, etl::StringView, std::string

    using BasicType = std::variant<JSON_BASIC_TYPES>;

    template<typename ...Ts>
    using List_ = etl::LinkedList<std::variant<JSON_BASIC_TYPES, Ts...>>;
    using List = List_<>;

    template<typename ...Ts>
    using Map_ = etl::UnorderedMap<std::string, std::variant<JSON_BASIC_TYPES, Ts...>>;
    using Map = Map_<>;

    #undef JSON_BASIC_TYPES
}


#define JSON_ITEM(k, v) etl::Pair<const char*, const etl::decay_t<decltype(m.v)>&>{k, m.v}

#define JSON_DEFINE_SERIALIZER(MODEL, ...) \
namespace Project::etl::json { \
    size_t size_max(const MODEL& m) { \
        return detail::json_max_size_variadic(__VA_ARGS__); \
    } \
    template <> \
    size_t size_max(Ref<const MODEL> m) { \
        return size_max(*m); \
    } \
    std::string serialize(const MODEL& m) { \
        std::string res; \
        res.reserve(size_max(m)); \
        res += '{'; \
        detail::json_append_variadic(res, __VA_ARGS__); \
        res.back() = '}'; \
        return res; \
    } \
    template <> \
    std::string serialize(Ref<const MODEL> m) { \
        return serialize(*m); \
    } \
}

#define JSON_DEFINE_DESERIALIZER(MODEL, ...) \
namespace Project::etl::json { \
    template <> \
    constexpr etl::Result<void, const char*> deserialize(etl::StringView j, MODEL& m) { \
        auto js = etl::Json::parse(j); \
        if (js.error_message()) return etl::Err(js.error_message().data()); \
        if (!js.is_dictionary()) return etl::Err("JSON is not a map"); \
        return detail::json_deserialize_variadic(js, __VA_ARGS__); \
    } \
}

#define JSON_DEFINE(MODEL, ...) \
JSON_DEFINE_SERIALIZER(MODEL, __VA_ARGS__) \
JSON_DEFINE_DESERIALIZER(MODEL, __VA_ARGS__) \

#endif