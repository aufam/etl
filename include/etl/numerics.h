#ifndef ETL_NUMERICS_H
#define ETL_NUMERICS_H

#include "etl/optional.h"
#include <cmath> // std::floor and std::trunc

namespace Project::etl::detail {
    
    template <typename R, typename T, typename U> using safe_arithmetic_operation_return_t = conditional_t
        <!is_void_v<R>, R, conditional_t
            <is_floating_point_v<T> || is_floating_point_v<U>, long double, conditional_t
                <is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, unsigned long long, long long>
            >
        >;

    template <typename T>
    struct DivMod { T quo, rem; };
}

namespace Project::etl {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"

    /// type safe less than
    template <typename T, typename U> enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>, bool> 
    safe_lt(T x, U y) {
        if constexpr (is_signed_integral_v<T> && is_unsigned_integral_v<U>)
            return x < T(0) ? true : x < y;
        if constexpr (is_unsigned_integral_v<T> && is_signed_integral_v<U>)
            return y < U(0) ? false : x < y;
        else
            return x < y;
    }

    /// type safe less equal
    template <typename T, typename U> enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>, bool> 
    safe_le(T x, U y) {
        if constexpr (is_signed_integral_v<T> && is_unsigned_integral_v<U>)
            return x < T(0) ? true : x <= y;
        if constexpr (is_unsigned_integral_v<T> && is_signed_integral_v<U>)
            return y < U(0) ? false : x <= y;
        else
            return x < y;
    }

    /// type safe greater than
    template <typename T, typename U> enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>, bool> 
    safe_gt(T x, U y) {
        if constexpr (is_signed_integral_v<T> && is_unsigned_integral_v<U>)
            return x < T(0) ? false : x > y;
        if constexpr (is_unsigned_integral_v<T> && is_signed_integral_v<U>)
            return y < U(0) ? true : x > y;
        else
            return x > y;
    }

    /// type safe greater equal
    template <typename T, typename U> enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>, bool> 
    safe_ge(T x, U y) {
        if constexpr (is_signed_integral_v<T> && is_unsigned_integral_v<U>)
            return x < T(0) ? false : x >= y;
        if constexpr (is_unsigned_integral_v<T> && is_signed_integral_v<U>)
            return y < U(0) ? true : x >= y;
        else
            return x >= y;
    }

    /// type safe equal
    template <typename T, typename U> enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>, bool> 
    safe_eq(T x, U y) {
        if constexpr (is_signed_integral_v<T> && is_unsigned_integral_v<U>)
            return x < T(0) ? false : x == y;
        if constexpr (is_unsigned_integral_v<T> && is_signed_integral_v<U>)
            return y < U(0) ? false : x == y;
        else
            return x == y;
    }

    /// type safe not equal
    template <typename T, typename U> enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>, bool> 
    safe_ne(T x, U y) {
        if constexpr (is_signed_integral_v<T> && is_unsigned_integral_v<U>)
            return x < T(0) ? true : x != y;
        if constexpr (is_unsigned_integral_v<T> && is_signed_integral_v<U>)
            return y < U(0) ? true : x != y;
        else
            return x != y;
    }

    /// type safe addition
    template <typename ReturnType = void, typename T, typename U,
              typename R = detail::safe_arithmetic_operation_return_t<ReturnType, T, U>
             > enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>, R>
    safe_add(T x, U y) { return static_cast<R>(x) + static_cast<R>(y); }

    /// type safe substraction
    template <typename ReturnType = void, typename T, typename U,
              typename R = detail::safe_arithmetic_operation_return_t<ReturnType, T, U>
             > enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>, R>
    safe_sub(T x, U y) { return static_cast<R>(x) - static_cast<R>(y); }
    
    /// type safe multiplication
    template <typename ReturnType = void, typename T, typename U,
              typename R = detail::safe_arithmetic_operation_return_t<ReturnType, T, U>
             > enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>, R>
    safe_mul(T x, U y) { return static_cast<R>(x) * static_cast<R>(y); }

    /// type safe division
    /// @note if R is integral, the return type is of Optional<R> and is valid if y is not 0  
    template <typename ReturnType = void, typename T, typename U,
              typename R = detail::safe_arithmetic_operation_return_t<ReturnType, T, U>
             > auto
    safe_div(T x, U y, enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>>* = nullptr) { 
        if constexpr (is_integral_v<R> && (is_floating_point_v<T> || is_floating_point_v<U>)) {
            auto res = static_cast<long double>(x) / static_cast<long double>(y);
            bool valid = !(std::isnan(res) || std::isinf(res));
            return valid ? etl::optional(static_cast<R>(res)) : etl::optional<R>();
        }
        else if constexpr (is_integral_v<R>)
            return y == U(0) ? etl::optional<R>() : etl::optional(static_cast<R>(x) / static_cast<R>(y));
        else
            return static_cast<R>(x) / static_cast<R>(y); 
    }    
    
    /// type safe floor division, calculate the floor of x / y 
    /// @note if R is integral, the return type is of Optional<R> and is valid if y is not 0  
    template <typename ReturnType = void, typename T, typename U,
              typename R = detail::safe_arithmetic_operation_return_t<ReturnType, T, U>
             > auto
    safe_floordiv(T x, U y, enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>>* = nullptr) { 
        if constexpr (is_integral_v<R>)
            return safe_div<R>(x, y);
        else
            return std::floor(static_cast<R>(x) / static_cast<R>(y)); 
    }

    /// type safe true division
    /// @note return type has to be floating point
    template <typename ReturnType = long double, typename T, typename U,
              typename R = detail::safe_arithmetic_operation_return_t<ReturnType, T, U>
             > enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U> && is_floating_point_v<R>, R>
    safe_truediv(T x, U y) { return safe_div<R>(x, y); }

    /// type safe modulo
    /// @note if R is integral, the return type is of Optional<R> and is valid if y is not 0  
    template <typename ReturnType = void, typename T, typename U,
              typename R = detail::safe_arithmetic_operation_return_t<ReturnType, T, U>
             > auto
    safe_mod(T x, U y, enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>>* = nullptr) {
        if constexpr (is_integral_v<R> && (is_floating_point_v<T> || is_floating_point_v<U>)) {
            auto quo = std::trunc(static_cast<long double>(x) / static_cast<long double>(y));
            auto rem = static_cast<long double>(x) - quo * static_cast<long double>(y);
            bool valid = !(std::isnan(quo) || std::isinf(quo));
            return valid ? etl::optional(static_cast<R>(rem)) : etl::optional<R>();
        }
        else if constexpr (is_integral_v<R>) {
            if (y == U(0)) return etl::optional<R>();
            auto remainder = static_cast<R>(x) % static_cast<R>(y);
            if (remainder < 0) remainder += y;
            return etl::optional(static_cast<R>(remainder));
        } 
        else {
            auto quotient = std::trunc(static_cast<R>(x) / static_cast<R>(y));
            auto remainder = static_cast<R>(x) - quotient * static_cast<R>(y);
            return remainder;
        }
    }

    /// type safe division and modulo
    /// @note if R is integral, the return type is of Optional<R> and is valid if y is not 0
    template <typename ReturnType = void, typename T, typename U,
              typename R = detail::safe_arithmetic_operation_return_t<ReturnType, T, U>
             > auto
    safe_divmod(T x, U y, enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>>* = nullptr) { 
        if constexpr (is_integral_v<R> && (is_floating_point_v<T> || is_floating_point_v<U>)) {
            detail::DivMod<R> res {};
            auto quo = std::trunc(static_cast<long double>(x) / static_cast<long double>(y));
            bool valid = !(std::isnan(quo) || std::isinf(quo));
            if (!valid) return etl::optional(res);

            res.quo = static_cast<R>(quo);
            res.rem = static_cast<R>(static_cast<long double>(x) - res.quo * static_cast<long double>(y));
            return valid ? etl::optional(static_cast<R>(res)) : etl::optional<R>();
        }
        if constexpr (is_integral_v<R>) {
            detail::DivMod<R> res {};
            if (y == U(0)) return etl::optional(res);

            res.quo = static_cast<R>(x) / static_cast<R>(y);
            res.rem = static_cast<R>(x) % static_cast<R>(y);
            if (res.rem < 0) res.rem += y;
            return etl::optional(res);
        } 
        else {
            detail::DivMod<R> res {};
            res.quo = std::trunc(static_cast<R>(x) / static_cast<R>(y));
            res.rem = static_cast<R>(x) - res.quo * static_cast<R>(y);
            return res;
        }
    }
#pragma GCC diagnostic pop

}

#endif