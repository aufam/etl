#ifndef ETL_JSON_SERIALIZE_H
#define ETL_JSON_SERIALIZE_H

#include "etl/json_size_max.h"

namespace Project::etl::detail {
    template <typename SB, typename S>
    void string_append(SB& buf, const S& other);

    template <typename S, typename K, typename V>
    void json_append(S& res, const K& key, const V& value);

    template <typename S, typename... Ts>
    void json_append_variadic(S& res, etl::Pair<const char*, const Ts&>... pairs);

    template <typename... Ts>
    size_t json_max_size_variadic(etl::Pair<const char*, const Ts&>... pairs);
}

namespace Project::etl::json {
    template <typename T, typename R = std::string> 
    R serialize(const T& value) {
        static_assert(etl::is_same_v<R, std::string> || etl::is_etl_string_v<R>, "Return type must be of type std::string or etl::String<N>");

        if constexpr (is_same_v<T, etl::None> || is_same_v<T, std::nullptr_t>) {
            return "null";
        } 
        else if constexpr (is_same_v<T, bool>) {
            return value ? "true" : "false";
        }
        else if constexpr (etl::is_integral_v<T>) {
            auto res = std::to_string(value);
            if constexpr (etl::is_same_v<R, std::string>)
                return res;
            else 
                return res.c_str();
        }
        else if constexpr (etl::is_floating_point_v<T>) {
            if (std::isnan(value) || std::isinf(value)) return "null";
            const char* format;
            if constexpr (etl::is_same_v<T, float>)
                format = "%.2f";
            else if constexpr (etl::is_same_v<T, double>)
                format = "%.3lf";
            else
                format = "%.4Lf";

            size_t n = size_max(value);

            if constexpr (etl::is_same_v<R, std::string>) {
                std::string res(n, '\0');
                const auto m = ::snprintf(&res[0], n + 1, format, value);
                res.resize(m);
                return res;
            } else {
                n = etl::min(n + 1, T::size());
                R res;
                ::snprintf(&res[0], n, format, value);
                return res;
            }
        }
        else if constexpr (etl::is_same_v<T, std::string> || etl::is_same_v<T, std::string_view> 
            || etl::is_etl_string_v<T> || etl::is_same_v<T, etl::StringView> || etl::is_same_v<T, const char*>
        ) {
            if constexpr (etl::is_same_v<R, std::string>) {
                std::string res;
                res.reserve(size_max(value));
                res += '\"';
                detail::string_append(res, value);
                res += '\"';
                return res;
            } else {
                R res;
                res += '\"';
                detail::string_append(res, value);
                res += '\"';
                return res;
            }
        }
        else if constexpr (etl::is_linked_list_v<T> || etl::is_vector_v<T> || etl::is_array_v<T> || 
            detail::is_std_list_v<T> || detail::is_std_vector_v<T> || detail::is_std_array_v<T>
        ) {
            const size_t n = size_max(value); 
            if (n == 2) return "[]";

            if constexpr (etl::is_same_v<R, std::string>) {
                std::string res;
                res.reserve(n);
                res += '[';
                for (const auto& item : value) {
                    res += serialize<etl::decay_t<decltype(item)>, R>(item);
                    res += ',';
                }
                res.back() = ']';
                return res;
            } else {
                R res;
                res += '[';
                for (const auto& item : value) {
                    res += serialize<etl::decay_t<decltype(item)>, R>(item);
                    res += ',';
                }
                res.back() = ']';
                return res;
            }
        }
        else if constexpr (etl::is_map_v<T> || etl::is_unordered_map_v<T> || 
            detail::is_std_map_v<T> || detail::is_std_unordered_map_v<T>
        ) {
            const size_t n = size_max(value); 
            if (n == 2) return "{}";

            if constexpr (etl::is_same_v<R, std::string>) {
                std::string res;
                res.reserve(n);
                res += '{';
                for (const auto& [k, v] : value) {
                    detail::json_append(res, k, v);
                }
                res.back() = '}';
                return res;
            } else { 
                R res;
                res += '{';
                for (const auto& [k, v] : value) {
                    detail::json_append(res, k, v);
                }
                res.back() = '}';
                return res;
            }
        }
        else if constexpr (detail::is_variant_v<T>) {
            return std::visit([&](const auto& item) -> R {
                return serialize<etl::decay_t<decltype(item)>, R>(item);
            }, value);
        }
        else if constexpr (etl::is_optional_v<T> || etl::is_ref_v<T> || detail::is_std_optional_v<T>) {
            return value ? serialize<etl::decay_t<decltype(*value)>, R>(*value) : "null";
        }
    }
}

namespace Project::etl::detail {
    template <typename SB, typename S>
    void string_append(SB& buf, const S& other) {
        if constexpr (etl::is_same_v<SB, std::string>) {
            if constexpr (etl::is_same_v<S, std::string> || etl::is_same_v<S, std::string_view> || etl::is_same_v<S, const char*>) {
                buf += other;
            } else if constexpr (etl::is_etl_string_v<S> || etl::is_same_v<S, etl::StringView>) {
                buf += std::string_view(other.data(), other.len());
            }
        } else if constexpr (etl::is_etl_string_v<SB>) { 
            if constexpr (etl::is_same_v<S, std::string> || etl::is_same_v<S, std::string_view>) {
                buf += etl::string_view(other.data(), other.size());
            } else if constexpr (etl::is_etl_string_v<S> || etl::is_same_v<S, etl::StringView> || etl::is_same_v<S, const char*>) {
                buf += other;
            }
        }
    }

    template <typename S, typename K, typename V>
    void json_append(S& res, const K& key, const V& value) {
        res += '\"';
        string_append(res, key);
        res += '\"';
        res += ':';
        res += json::serialize<V, S>(value);
        res += ',';
    }

    template <typename S, typename... Ts>
    void json_append_variadic(S& res, etl::Pair<const char*, const Ts&>... pairs) {
        if constexpr (sizeof...(pairs) == 0) {
            res += '}';
        } else {
            (json_append(res, pairs.x, pairs.y), ...);
        }
    }

    template <typename... Ts>
    size_t json_max_size_variadic(etl::Pair<const char*, const Ts&>... pairs) {
        if constexpr (sizeof...(pairs) == 0) {
            return 2;
        } else {
            size_t res = 1;
            res += ((json::size_max(pairs.x) + 1 + json::size_max(pairs.y) + 1) + ...);
            return res;
        }
    }
}

#endif
