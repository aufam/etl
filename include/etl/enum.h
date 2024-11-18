#ifndef ETL_ENUM_H
#define ETL_ENUM_H

#include "etl/utility_basic.h"
#include "etl/tuple.h"
#include <variant>

namespace Project::etl {
    template <typename ...Ts>
    class Enum {
    public:
        std::variant<Ts...> variant;

        Enum() = delete;

        Enum(const Enum<Ts...>&) = default;
        Enum(Enum<Ts...>&&) = default;
        Enum& operator=(const Enum<Ts...>&) = default;
        Enum& operator=(Enum<Ts...>&&) = default;
        
        template <typename T> constexpr 
        Enum(T&& value) : variant(etl::forward<T>(value)) {}

        template <typename T> constexpr Enum&
        operator=(T&& value) { return (variant = etl::forward<T>(value), *this); }

        template <typename T> constexpr etl::enable_if_t<(etl::is_same_v<T, Ts> || ...), bool>
        operator==(const T& value) const { return std::holds_alternative<T>(variant) && std::get<T>(variant) == value; } 

        template <typename T> constexpr T&
        as() & { return std::get<T>(variant); }

        template <typename T> constexpr const T&
        as() const & { return std::get<T>(variant); }

        template <typename T> constexpr T&&
        as() && { return etl::move(std::get<T>(variant)); }

        template <typename T> constexpr const T&&
        as() const && { return etl::move(std::get<T>(variant)); }
    };
}

#endif