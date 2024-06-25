#ifndef ETL_JSON_SERIALIZE_H
#define ETL_JSON_SERIALIZE_H

#include "etl/json_size_max.h"

namespace Project::etl::detail {
    template <typename K, typename V>
    void json_append(std::string& res, const K& key, const V& value);

    template <typename... Ts>
    void json_append_variadic(std::string& res, etl::Pair<const char*, const Ts&>... pairs);

    template <typename... Ts>
    size_t json_max_size_variadic(etl::Pair<const char*, const Ts&>... pairs);
}

namespace Project::etl::json {
    // prototypes
    template <typename T> std::string serialize(const etl::Vector<T>&);
    template <typename T> std::string serialize(const etl::LinkedList<T>&);
    template <typename T> std::string serialize(const etl::Map<std::string, T>&);
    template <typename T> std::string serialize(const etl::UnorderedMap<std::string, T>&);
    template <typename T> std::string serialize(const etl::Optional<T>&);
    template <typename... Ts> std::string serialize(const std::variant<Ts...>&);
    template <typename T, typename F> std::string serialize(const etl::Getter<T, F>&);
    template <typename T, typename GF, typename SF> std::string serialize(const etl::GetterSetter<T, GF, SF>&);
    template <typename T> std::string serialize(etl::Ref<T>);

    // null
    inline std::string serialize(etl::None) {
        return "null";
    }

    inline std::string serialize(std::nullptr_t) {
        return "null";
    }

    // bool
    inline std::string serialize(bool value) {
        return value ? "true" : "false";
    }

    // number
    template <typename T, typename = etl::enable_if_t<etl::is_integral_v<T>>>
    std::string serialize(T value) {
        return std::to_string(value);
    }

    inline std::string serialize(float value) {
        if (std::isnan(value) || std::isinf(value)) return "null";
        const size_t n = size_max(value);
        std::string res(n, '\0');
        const auto m = std::snprintf(&res[0], n + 1, "%.2f", value);
        res.resize(m);
        return res;
    }

    inline std::string serialize(double value) {
        if (std::isnan(value) || std::isinf(value)) return "null";
        const size_t n = size_max(value);
        std::string res(n, '\0');
        const auto m = std::snprintf(&res[0], n + 1, "%.4lf", value);
        res.resize(m);
        return res;
    }

    inline std::string serialize(long double value) {
        if (std::isnan(value) || std::isinf(value)) return "null";
        const size_t n = size_max(value);
        std::string res(n, '\0');
        const auto m = std::snprintf(&res[0], n + 1, "%.6Lf", value);
        res.resize(m);
        return res;
    }

    // string
    inline std::string serialize(const std::string& value) {
        std::string res;
        res.reserve(size_max(value));
        res += '\"';
        res += value;
        res += '\"';
        return res;
    }

    inline std::string serialize(std::string_view value) {
        std::string res;
        res.reserve(size_max(value));
        res += '\"';
        res += value;
        res += '\"';
        return res;
    }

    template <size_t N>
    std::string serialize(const etl::String<N>& value) {
        std::string res;
        res.reserve(size_max(value));
        res += '\"';
        res += std::string(value.data(), value.len());
        res += '\"';
        return res;
    }

    inline std::string serialize(etl::StringView value) {
        std::string res;
        res.reserve(size_max(value));
        res += '\"';
        res += std::string(value.data(), value.len());
        res += '\"';
        return res;
    }

    inline std::string serialize(const char* value) {
        std::string res;
        res.reserve(size_max(value));
        res += '\"';
        res += std::string(value);
        res += '\"';
        return res;
    }

    // list
    template <typename T>
    std::string serialize(const etl::Vector<T>& value) {
        if (value.len() == 0) return "[]";
        std::string res;
        res.reserve(size_max(value));
        res += '[';
        for (auto& item : value) {
            res += serialize(item);
            res += ',';
        }
        res.back() = ']';
        return res;
    }

    template <typename T>
    std::string serialize(const etl::LinkedList<T>& value) {
        if (value.len() == 0) return "[]";
        std::string res;
        res.reserve(size_max(value));
        res += '[';
        for (auto& item : value) {
            res += serialize(item);
            res += ',';
        }
        res.back() = ']';
        return res;
    }

    // dict
    template <typename T>
    std::string serialize(const etl::Map<std::string, T>& value) {
        if (value.len() == 0) return "{}";
        std::string res;
        res.reserve(size_max(value));
        res += '{';
        for (auto [k, v] : value) {
            detail::json_append(res, k, v);
        }
        res.back() = '}';
        return res;
    }

    template <typename T>
    std::string serialize(const etl::UnorderedMap<std::string, T>& value) {
        if (value.len() == 0) return "{}";
        std::string res;
        res.reserve(size_max(value));
        res += '{';
        for (auto [k, v] : value) {
            detail::json_append(res, k, v);
        }
        res.back() = '}';
        return res;
    }

    // variant
    template <typename ...Ts>
    std::string serialize(const std::variant<Ts...>& value) {
        return std::visit([&](const auto& item) -> std::string {
            return serialize(item);
        }, value);
    }

    // null or else
    template <typename T>
    std::string serialize(const etl::Optional<T>& value) {
        return value ? serialize(*value) : "null";
    }

    // getter
    template <typename T, typename F>
    std::string serialize(const etl::Getter<T, F>& value) {
        return serialize(value.get());
    }

    // getter setter
    template <typename T, typename GF, typename SF>
    std::string serialize(const etl::GetterSetter<T, GF, SF>& value) {
        return serialize(value.get());
    }

    template <typename T> 
    std::string serialize(etl::Ref<T> value) {
        return value ? serialize(*value) : "null";
    }
}

namespace Project::etl::detail {
    template <typename K, typename V>
    void json_append(std::string& res, const K& key, const V& value) {
        res += '\"';
        res += key;
        res += '\"';
        res += ':';
        res += json::serialize(value);
        res += ',';
    }

    template <typename... Ts>
    void json_append_variadic(std::string& res, etl::Pair<const char*, const Ts&>... pairs) {
        if constexpr (sizeof...(pairs) == 0) {
            res += '}';
        } else {
            (json_append(res, pairs.x, etl::ref_const(pairs.y)), ...);
        }
    }

    template <typename... Ts>
    size_t json_max_size_variadic(etl::Pair<const char*, const Ts&>... pairs) {
        if constexpr (sizeof...(pairs) == 0) {
            return 2;
        } else {
            size_t res = 2;
            res += ((json::size_max(pairs.x) + 1 + json::size_max(etl::ref_const(pairs.y)) + 1) + ...);
            return res;
        }
    }
}

#endif
