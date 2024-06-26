#ifndef ETL_JSON_SIZE_MAX_H
#define ETL_JSON_SIZE_MAX_H

#include "etl/json_types.h"

namespace Project::etl::json {
    template <typename T>
    size_t size_max(const T& value) {
        if constexpr (is_same_v<T, etl::None> || is_same_v<T, std::nullptr_t>) {
            return 4;
        } 
        else if constexpr (is_same_v<T, bool>) {
            return 5;
        }
        else if constexpr (etl::is_integral_v<T>) {
            if (value < 0) return size_max(-value) + 1;
            if (value < 10) return 1;
            return size_max(value / 10);
        }
        else if constexpr (etl::is_floating_point_v<T>) {
            size_t decimal;
            if constexpr (etl::is_same_v<T, float>)
                decimal = 3;
            else if constexpr (etl::is_same_v<T, double>)
                decimal = 5;
            else
                decimal = 7;
            return size_max(ssize_t(value)) + decimal;
        }
        else if constexpr (etl::is_same_v<T, const char*>) {
            return ::strlen(value) + 2;
        }
        else if constexpr (etl::is_same_v<T, std::string> || etl::is_same_v<T, std::string_view>) {
            return value.size() + 2;
        }
        else if constexpr (etl::is_string_v<T> || etl::is_same_v<T, etl::StringView>) {
            return value.len() + 2;
        }
        else if constexpr (etl::is_linked_list_v<T> || etl::is_vector_v<T>) {
            size_t cnt = 2;
            for (auto& item : value) {
                cnt += size_max(item) + 1;
            }
            return cnt;
        }
        else if constexpr (etl::is_map_v<T> || etl::is_unordered_map_v<T>) {
            size_t cnt = 2;
            for (const auto &[k, v] : value) {
                cnt += size_max(k) + 1 + size_max(v) + 1;
            }
            return cnt;
        }
        else if constexpr (detail::is_variant_v<T>) {
            return std::visit([&](const auto& item) -> size_t {
                return size_max(item);
            }, value);
        }
        else if constexpr (etl::is_optional_v<T> || etl::is_ref_v<T>) {
            return value ? size_max(*value) : 4;
        }
        else if constexpr (detail::is_getter_v<T> || detail::is_getter_setter_v<T>) {
            return size_max(value.get());
        }
    }

    inline size_t size_max(const char* value) {
        return ::strlen(value) + 2;
    }
}

#endif
