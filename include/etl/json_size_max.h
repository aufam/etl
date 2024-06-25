#ifndef ETL_JSON_SIZE_MAX_H
#define ETL_JSON_SIZE_MAX_H

#include "etl/json_types.h"

namespace Project::etl::json {
    // prototypes
    template <typename T> size_t size_max(const etl::Vector<T>&);
    template <typename T> size_t size_max(const etl::LinkedList<T>&);
    template <typename T> size_t size_max(const etl::Map<std::string, T>&);
    template <typename T> size_t size_max(const etl::UnorderedMap<std::string, T>&);
    template <typename T> size_t size_max(const etl::Optional<T>&);
    template <typename... Ts> size_t size_max(const std::variant<Ts...>&);
    template <typename T, typename F> size_t size_max(const etl::Getter<T, F>&);
    template <typename T, typename GF, typename SF> size_t size_max(const etl::GetterSetter<T, GF, SF>&);
    template <typename T> size_t size_max(etl::Ref<T>);

    // null
    inline constexpr size_t size_max(etl::None) {
        return 4;
    }

    inline constexpr size_t size_max(std::nullptr_t) {
        return 4;
    }

    // bool
    inline constexpr size_t size_max(bool) {
        return 5;
    }

    // number
    template <typename T, typename = etl::enable_if_t<etl::is_integral_v<T>>>
    constexpr size_t size_max(T value) {
        if (value < 0) return size_max(-value) + 1;
        if (value < 10) return 1;
        return size_max(value / 10);
    }

    inline constexpr size_t size_max(float value) {
        return size_max(ssize_t(value)) + 3;
    }

    inline constexpr size_t size_max(double value) {
        return size_max(ssize_t(value)) + 5;
    }

    inline constexpr size_t size_max(long double value) {
        return size_max(ssize_t(value)) + 7;
    }

    // string
    inline size_t size_max(const std::string& value) {
        return value.size() + 2;
    }

    inline constexpr size_t size_max(std::string_view value) {
        return value.size() + 2;
    }

    template <size_t N>
    inline constexpr size_t size_max(const etl::String<N>& value) {
        return value.len() + 2;
    }

    inline constexpr size_t size_max(etl::StringView value) {
        return value.len() + 2;
    }

    inline size_t size_max(const char* value) {
        return ::strlen(value) + 2;
    }

    // list
    template <typename T>
    size_t size_max(const etl::Vector<T>& value) {
        size_t cnt = 2;
        for (const auto& item : value) {
            cnt += size_max(item) + 1;
        }
        return cnt;
    }

    template <typename T>
    size_t size_max(const etl::LinkedList<T>& value) {
        size_t cnt = 2;
        for (const auto& item : value) {
            cnt += size_max(item) + 1;
        }
        return cnt;
    }

    // dict
    template <typename T>
    size_t size_max(const etl::Map<std::string, T>& value) {
        size_t cnt = 2;
        for (const auto &[k, v] : value) {
            cnt += size_max(k) + 1 + size_max(v) + 1;
        }
        return cnt;
    }

    template <typename T>
    size_t size_max(const etl::UnorderedMap<std::string, T>& value) {
        size_t cnt = 2;
        for (const auto &[k, v] : value) {
            cnt += size_max(k) + 1 + size_max(v) + 1;
        }
        return cnt;
    }

    // null or else
    template <typename T>
    size_t size_max(const etl::Optional<T>& value) {
        return value ? size_max(*value) : 4;
    }

    // variant
    template <typename ...Ts>
    size_t size_max(const std::variant<Ts...>& value) {
        return std::visit([&](const auto& arg) -> size_t { return size_max(arg); }, value);
    }

    // getter
    template <typename T, typename F>
    size_t size_max(const etl::Getter<T, F>& value) {
        return size_max(value.get());
    }

    template <typename T, typename GF, typename SF>
    size_t size_max(const etl::GetterSetter<T, GF, SF>& value) {
        return size_max(value.get());
    }

    template <typename T>
    size_t size_max(etl::Ref<T> value) {
        return value ? size_max(*value) : 4;
    }
}

#endif
