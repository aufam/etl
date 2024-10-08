#ifndef ETL_JSON_SIZE_MAX_H
#define ETL_JSON_SIZE_MAX_H

#include "etl/json_types.h"
#include <cmath>

namespace Project::etl::json {
    template <typename T>
    size_t size_max(const T& value) {
        if constexpr (is_same_v<T, etl::None> || is_same_v<T, std::nullptr_t>) {
            return 4;
        } 
        else if constexpr (is_same_v<T, bool>) {
            return value ? 4 : 5;
        }
        else if constexpr (etl::is_integral_v<T>) {
            if (value < 0) return size_max(-value) + 1;
            if (value < 10) return 1;
            return size_max(value / 10) + 1;
        }
        else if constexpr (etl::is_floating_point_v<T>) {
            size_t scale;
            size_t decimal;
            if constexpr (etl::is_same_v<T, float>) {
                scale = 100;
                decimal = 2;
            }
            else if constexpr (etl::is_same_v<T, double>) {
                scale = 1000;
                decimal = 3;
            }
            else {
                scale = 10000;
                decimal = 4;
            }

            if (std::abs(value) < 1.0) {
                return decimal + 2 + std::signbit(value);
            }
            return size_max(ssize_t(value * scale)) + 1;
        }
        else if constexpr (etl::is_same_v<T, const char*>) {
            return ::strlen(value) + 2;
        }
        else if constexpr (etl::is_same_v<T, std::string> || etl::is_same_v<T, std::string_view>) {
            return value.size() + 2;
        }
        else if constexpr (etl::is_etl_string_v<T> || etl::is_same_v<T, etl::StringView>) {
            return value.len() + 2;
        }
        else if constexpr (etl::is_linked_list_v<T> || etl::is_vector_v<T> || etl::is_array_v<T> || 
            detail::is_std_list_v<T> || detail::is_std_vector_v<T> || detail::is_std_array_v<T>
        ) {
            size_t cnt = 1;
            bool is_empty = true;
            for (const auto& item : value) {
                is_empty = false;
                cnt += size_max(item) + 1;
            }
            return cnt + is_empty;
        }
        else if constexpr (etl::is_map_v<T> || etl::is_unordered_map_v<T> || 
            detail::is_std_map_v<T> || detail::is_std_unordered_map_v<T>
        ) {
            size_t cnt = 1;
            bool is_empty = true;
            for (const auto &[k, v] : value) {
                is_empty = false;
                cnt += size_max(k) + 1 + size_max(v) + 1;
            }
            return cnt + is_empty;
        }
        else if constexpr (detail::is_variant_v<T>) {
            return std::visit([&](const auto& item) -> size_t {
                return size_max(item);
            }, value);
        }
        else if constexpr (etl::is_optional_v<T> || etl::is_ref_v<T> || detail::is_std_optional_v<T>) {
            return value ? size_max(*value) : 4;
        } else {
            return 0;
        }
    }

    inline size_t size_max(const char* value) {
        return ::strlen(value) + 2;
    }
}

#endif
