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
    using Map_ = etl::UnorderedMap<etl::StringView, std::variant<JSON_BASIC_TYPES, Ts...>>;
    using Map = Map_<>;

    #undef JSON_BASIC_TYPES
}

namespace Project::etl::detail {
    template <typename T> struct is_variant : false_type {};
    template <typename... T> struct is_variant<std::variant<T...>> : true_type {};
    template <typename T> inline constexpr bool is_variant_v = is_variant<T>::value;

    template <typename T> struct is_getter : false_type {};
    template <typename T, typename GF> struct is_getter<etl::Getter<T, GF>> : true_type {};
    template <typename T> inline constexpr bool is_getter_v = is_getter<T>::value;

    template <typename T> struct is_getter_setter : false_type {};
    template <typename T, typename GF, typename SF> struct is_getter_setter<etl::GetterSetter<T, GF, SF>> : true_type {};
    template <typename T> inline constexpr bool is_getter_setter_v = is_getter_setter<T>::value;
}


#define JSON_ITEM(k, v) etl::Pair<const char*, const etl::decay_t<decltype(m.v)>&>{k, m.v}

#define JSON_DEFINE_SERIALIZER(MODEL, ...) \
namespace Project::etl::json { \
    template <> \
    size_t size_max(const MODEL& m) { \
        return detail::json_max_size_variadic(__VA_ARGS__); \
    } \
    template <> \
    std::string serialize(const MODEL& m) { \
        std::string res; \
        res.reserve(size_max(m)); \
        res += '{'; \
        detail::json_append_variadic(res, __VA_ARGS__); \
        res.back() = '}'; \
        return res; \
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