#ifndef ETL_RESULT_H
#define ETL_RESULT_H

#include "etl/optional.h"
#include <variant>

namespace Project::etl {
    /// Represents a variant type that holds an ok value. the type can be void
    /// @tparam T The type of the ok value or void if not specified.
    template <typename... T>
    class Ok {
    public:
        using type = typename std::tuple_element<sizeof...(T), std::tuple<None, T...>>::type;
        static_assert(!etl::is_reference_v<type>, "Ok must have no reference alternative.");
        static_assert(sizeof...(T) <= 1, "Ok can't hold multiple types.");

        type data;
        constexpr Ok(T... arg) : data(etl::move(arg)...) {}
    };

    /// Represents a variant type that holds an error value of a specified type.
    /// @tparam E The type of the error value.
    template <typename E>
    class Err {
    public:
        using type = E;
        static_assert(!etl::is_reference_v<type>, "Err must have no reference alternative.");
        static_assert(!etl::is_void_v<E>, "Error variant can't be void");

        type data;
        constexpr Err(E data) : data(etl::move(data)) {}
    };

    /// Represents a result type that can hold either a value of type T or an error of type E.
    /// Result objects cannot be copied. Instead it will move the resource to the new Result object
    /// @tparam T The type of the value.
    /// @tparam E The type of the error.
    template <typename T, typename E>
    class [[nodiscard]] Result {
    public:
        using value_type = T;
        using error_type = E;

        /// Constructs a Result object from an Ok variant.
        template <typename U, typename = enable_if_t<etl::is_convertible_v<U, T>>>
        Result(const Ok<U>& value) : variant(Ok<T>(static_cast<T>(value.data))) {}

        /// Constructs a Result object from an Err variant.
        template <typename U, typename = enable_if_t<etl::is_convertible_v<U, E>>>
        Result(const Err<U>& value) : variant(Err<E>(static_cast<E>(value.data))) {}

        /// Constructs a Result object from an Ok variant.
        template <typename U, typename = enable_if_t<etl::is_convertible_v<U, T>>>
        Result(Ok<U>&& value) : variant(Ok<T>(static_cast<T>(etl::move(value.data)))) {}

        /// Constructs a Result object from an Err variant.
        template <typename U, typename = enable_if_t<etl::is_convertible_v<U, E>>>
        Result(Err<U>&& value) : variant(Err<E>(static_cast<E>(etl::move(value.data)))) {}

        Result(const Result& other) = default;
        Result(Result&& other) = default;

        template <typename U, typename = enable_if_t<etl::is_convertible_v<U, T>>>
        Result(const Result<U, E>& other) : variant() {
            if (other.variant.index() == 1) variant = Ok<T>(static_cast<T>(other.unwrap()));
            if (other.variant.index() == 2) variant = Err<E>(static_cast<E>(other.unwrap_err()));
        }

        template <typename U, typename = enable_if_t<etl::is_convertible_v<U, T>>>
        Result(Result<U, E>&& other) : variant() {
            if (other.variant.index() == 1) variant = Ok<T>(static_cast<T>(etl::move(other.unwrap())));
            if (other.variant.index() == 2) variant = Err<E>(static_cast<E>(etl::move(other.unwrap_err())));
        }

        T& unwrap() & { return std::get<Ok<T>>(variant).data; }

        T&& unwrap() && { return etl::move(std::get<Ok<T>>(variant).data); }
        
        const T& unwrap() const& { return std::get<Ok<T>>(variant).data; }

        const T&& unwrap() const&& { return etl::move(std::get<Ok<T>>(variant).data); }

        template <typename U>
        T unwrap_or(U&& value) && { 
            return variant.index() == 1 ? etl::move(std::get<Ok<T>>(variant).data) : static_cast<T>(etl::forward<U>(value)); 
        }

        template <typename U>
        T unwrap_or(U&& value) const& { 
            return variant.index() == 1 ? std::get<Ok<T>>(variant).data : static_cast<T>(etl::forward<U>(value)); 
        }

        E& unwrap_err() & { return std::get<Err<E>>(variant).data; }

        E&& unwrap_err() && { return etl::move(std::get<Err<E>>(variant).data); }
        
        const E& unwrap_err() const& { return std::get<Err<E>>(variant).data; }

        const E&& unwrap_err() const&& { return etl::move(std::get<Err<E>>(variant).data); }

        template <typename U>
        E unwrap_err_or(U&& value) && { 
            return variant.index() == 2 ? etl::move(std::get<Err<E>>(variant).data) : static_cast<E>(etl::forward<U>(value)); 
        }

        template <typename U>
        E unwrap_err_or(U&& value) const& { 
            return variant.index() == 2 ? std::get<Err<E>>(variant).data : static_cast<E>(etl::forward<U>(value)); 
        }

        /// structured binding support
        template <size_t I>
        auto get() & {
            if constexpr (I == 0) {
                return variant.index() == 1 ? Optional<T&>(unwrap()) : Optional<T&>();
            } else if constexpr (I == 1) {
                return variant.index() == 2 ? Optional<E&>(unwrap_err()) : Optional<E&>();
            }
        }

        /// structured binding support
        template <size_t I>
        auto get() && {
            if constexpr (I == 0) {
                return variant.index() == 1 ? Optional<T>(etl::move(unwrap())) : Optional<T>();
            } else if constexpr (I == 1) {
                return variant.index() == 2 ? Optional<E>(etl::move(unwrap_err())) : Optional<E>();
            }
        }

        /// structured binding support
        template <size_t I>
        auto get() const& {
            if constexpr (I == 0) {
                return variant.index() == 1 ? Optional<const T&>(unwrap()) : Optional<const T&>();
            } else if constexpr (I == 1) {
                return variant.index() == 2 ? Optional<const E&>(unwrap_err()) : Optional<const E&>();
            }
        }

        /// Chains a function call on Ok variant.
        template<typename F, typename R = decltype(etl::declval<F>()(etl::declval<T>()))>
        Result<R, E> then(F&& fn) && {
            switch (variant.index()) {
                case 1: 
                    if constexpr (etl::is_void_v<R>) return (fn(std::move(unwrap())), Ok());
                    else return Ok<R>(fn(std::move(unwrap())));
                case 2: return Err<E>(std::move(unwrap_err()));
                default: return {};
            }
        }

        /// Chains a function call on Ok variant.
        template<typename F, typename R = decltype(etl::declval<F>()(etl::declval<T>()))>
        Result<R, E> then(F&& fn) const& {
            switch (variant.index()) {
                case 1: 
                    if constexpr (etl::is_void_v<R>) return (fn(unwrap()), Ok());
                    else return Ok<R>(fn(unwrap()));
                case 2: return Err<E>(unwrap_err());
                default: return {};
            }
        }

        /// Chains a function call on Ok variant.
        template<typename F, typename R = decltype(etl::declval<F>()(etl::declval<T>()))>
        R and_then(F&& fn) && {
            switch (variant.index()) {
                case 1: return fn(etl::move(unwrap()));
                case 2: return Err<E>(etl::move(unwrap_err()));
                default: return {};
            }
        }

        /// Chains a function call on Ok variant.
        template<typename F, typename R = decltype(etl::declval<F>()(etl::declval<T>()))>
        R and_then(F&& fn) const& {
            switch (variant.index()) {
                case 1: return fn(unwrap());
                case 2: return Err<E>(unwrap_err());
                default: return {};
            }
        }

        /// Chains a function call on Err variant.
        template<typename F, typename R = decltype(etl::declval<F>()(etl::declval<E>()))>
        Result<T, R> except(F&& fn) && {
            switch (variant.index()) {
                case 2: return Err<R>(fn(etl::move(unwrap_err())));
                case 1: return Ok<T>(etl::move(unwrap()));
                default: return {};
            }
        }

        /// Chains a function call on Err variant.
        template<typename F, typename R = decltype(etl::declval<F>()(etl::declval<E>()))>
        Result<T, R> except(F&& fn) const& {
            switch (variant.index()) {
                case 2: return Err<R>(fn(unwrap_err()));
                case 1: return Ok<T>(unwrap());
                default: return {};
            }
        }

        /// Chains a function call on Err variant.
        template<typename F, typename R = decltype(etl::declval<F>()(etl::declval<E>()))>
        R or_except(F&& fn) && {
            switch (variant.index()) {
                case 2: return fn(etl::move(unwrap_err()));
                case 1: return Ok<T>(etl::move(unwrap()));
                default: return {};
            }
        }

        /// Chains a function call on Err variant.
        template<typename F, typename R = decltype(etl::declval<F>()(etl::declval<E>()))>
        R or_except(F&& fn) const& {
            switch (variant.index()) {
                case 2: return fn(unwrap_err());
                case 1: return Ok<T>(unwrap());
                default: return {};
            }
        }

    private:
        Result() = default;

        std::variant<None, Ok<T>, Err<E>> variant;

        template <typename TT, typename EE>
        friend class Result;
    };

    template <typename E>
    class Result<void, E> {
    public:
        using value_type = void;
        using error_type = E;

        Result(const Ok<>&) : variant(Ok<>()) {}

        /// Constructs a Result object from an Err variant.
        template <typename U, typename = enable_if_t<etl::is_convertible_v<U, E>>>
        Result(const Err<U>& value) : variant(value) {}

        /// Constructs a Result object from an Err variant.
        template <typename U, typename = enable_if_t<etl::is_convertible_v<U, E>>>
        Result(Err<U>&& value) : variant(Err<E>(static_cast<E>(etl::move(value.data)))) {}

        Result(const Result& other) = default;
        Result(Result&& other) = default;

        template <typename U>
        Result(const Result<U, E>& other) : variant() {
            switch (other.variant.index()) {
                case 1: variant = Ok<>(); return;
                case 2: variant = Err<E>(other.unwrap_err()); return;
                default: return;
            }
        }

        template <typename U>
        Result(Result<U, E>&& other) : variant() {
            switch (other.variant.index()) {
                case 1: variant = Ok<>(); return;
                case 2: variant = Err<E>(etl::move(other.unwrap_err())); return;
                default: return;
            }
        }

        void unwrap() const { return; }

        E& unwrap_err() & { return std::get<Err<E>>(variant).data; }

        E&& unwrap_err() && { return etl::move(std::get<Err<E>>(variant).data); }
        
        const E& unwrap_err() const& { return std::get<Err<E>>(variant).data; }

        const E&& unwrap_err() const&& { return etl::move(std::get<Err<E>>(variant).data); }

        template <typename U>
        E unwrap_err_or(U&& value) && { 
            return variant.index() == 2 ? etl::move(std::get<Err<E>>(variant).data) : static_cast<E>(etl::forward<U>(value)); 
        }

        template <typename U>
        E unwrap_err_or(U&& value) const& { 
            return variant.index() == 2 ? std::get<Err<E>>(variant).data : static_cast<E>(etl::forward<U>(value)); 
        }
        
        template <size_t I>
        auto get() & {
            if constexpr (I == 0) {
                return variant.index() == 1;
            } else if constexpr (I == 1) {
                return variant.index() == 2 ? Optional<E&>(unwrap_err()) : Optional<E&>();
            }
        }
        
        template <size_t I>
        auto get() && {
            if constexpr (I == 0) {
                return variant.index() == 1;
            } else if constexpr (I == 1) {
                return variant.index() == 2 ? Optional<E>(etl::move(unwrap_err())) : Optional<E>();
            }
        }

        template <size_t I>
        auto get() const& {
            if constexpr (I == 0) {
                return variant.index() == 1;
            } else if constexpr (I == 1) {
                return variant.index() == 2 ? Optional<const E&>(unwrap_err()) : Optional<const E&>();
            }
        }

        template<typename F, typename R = decltype(etl::declval<F>()())>
        Result<R, E> then(F&& fn) && {
            switch (variant.index()) {
                case 1: if constexpr (etl::is_void_v<R>) return (fn(), Ok());
                        else return Ok<R>(fn());
                case 2: return Err<E>(etl::move(unwrap_err()));
                default: return {};
            }
        }

        template<typename F, typename R = decltype(etl::declval<F>()())>
        Result<R, E> then(F&& fn) const& {
            switch (variant.index()) {
                case 1: if constexpr (etl::is_void_v<R>) return (fn(), Ok());
                        else return Ok<R>(fn());
                case 2: return Err<E>(unwrap_err());
                default: return {};
            }
        }

        template<typename F, typename R = decltype(etl::declval<F>()())>
        R and_then(F&& fn) && {
            switch (variant.index()) {
                case 1: return fn();
                case 2: return Err<E>(etl::move(unwrap_err()));
                default: return {};
            }
        }

        template<typename F, typename R = decltype(etl::declval<F>()())>
        R and_then(F&& fn) const& {
            switch (variant.index()) {
                case 1: return fn();
                case 2: return Err<E>(unwrap_err());
                default: return {};
            }
        }

        template<typename F, typename R = decltype(etl::declval<F>()(etl::declval<E>()))>
        Result<void, R> except(F&& fn) && {
            switch (variant.index()) {
                case 2: return Err<R>(fn(etl::move(unwrap_err())));
                case 1: return Ok();
                default: return {};
            }
        }

        template<typename F, typename R = decltype(etl::declval<F>()(etl::declval<E>()))>
        Result<void, R> except(F&& fn) const& {
            switch (variant.index()) {
                case 2: return Err<R>(fn(unwrap_err()));
                case 1: return Ok();
                default: return {};
            }
        }

        template<typename F, typename R = decltype(etl::declval<F>()(etl::declval<E>()))>
        R or_except(F&& fn) && {
            switch (variant.index()) {
                case 2: return fn(etl::move(unwrap_err()));
                case 1: return Ok();
                default: return {};
            }
        }

        template<typename F, typename R = decltype(etl::declval<F>()(etl::declval<E>()))>
        R or_except(F&& fn) const& {
            switch (variant.index()) {
                case 2: return fn(unwrap_err());
                case 1: return Ok();
                default: return {};
            }
        }

    private:
        Result() {}

        std::variant<None, Ok<>, Err<E>> variant;

        template <typename TT, typename EE>
        friend class Result;
    };

    template <typename R> struct is_etl_result : false_type {};
    template <typename T, typename E> struct is_etl_result<Result<T, E>> : true_type {};
    template <typename R> inline constexpr bool is_etl_result_v = is_etl_result<R>::value;

    template <typename R> struct result_value {};
    template <typename T, typename E> struct result_value<Result<T, E>> { typedef T type; };
    template <typename R> using result_value_t = typename result_value<R>::type;
    
    template <typename R> struct result_error {};
    template <typename T, typename E> struct result_error<Result<T, E>> { typedef E type; };
    template <typename R> using result_error_t = typename result_error<R>::type;
    

#define ETL_RESULT_ARITHMETICS(op) \
    template <typename R, typename U, typename = etl::enable_if_t< \
        etl::is_etl_result_v<etl::decay_t<R>> && etl::is_arithmetic_v<etl::decay_t<U>> \
    >> auto operator op (R&& res, U&& other) { \
        using result_type = result_value_t<etl::decay_t<R>>; \
        if constexpr (etl::is_reference_v<result_value_t<etl::decay_t<R>>>) { \
            return res.then([other] (result_type value) -> decltype(auto) { return value op other; }); \
        } else { \
            return res.then([other] (result_type value) -> auto { return value op other; }); \
        } \
    } \
    template <typename R, typename U, typename = etl::enable_if_t< \
        etl::is_etl_result_v<etl::decay_t<R>> && etl::is_arithmetic_v<etl::decay_t<U>> \
    >> auto operator op (U&&other, R&& res) { \
        using result_type = result_value_t<etl::decay_t<R>>; \
        return res.then([other] (result_type value) mutable -> decltype(auto) { return other op value; }); \
    }

    ETL_RESULT_ARITHMETICS(+)
    ETL_RESULT_ARITHMETICS(-)
    ETL_RESULT_ARITHMETICS(*)
    ETL_RESULT_ARITHMETICS(/)
    ETL_RESULT_ARITHMETICS(&)
    ETL_RESULT_ARITHMETICS(|)

    ETL_RESULT_ARITHMETICS(+=)
    ETL_RESULT_ARITHMETICS(-=)
    ETL_RESULT_ARITHMETICS(*=)
    ETL_RESULT_ARITHMETICS(/=)
    ETL_RESULT_ARITHMETICS(&=)
    ETL_RESULT_ARITHMETICS(|=)

    ETL_RESULT_ARITHMETICS(&&)
    ETL_RESULT_ARITHMETICS(||)
    
    ETL_RESULT_ARITHMETICS(>)
    ETL_RESULT_ARITHMETICS(<)
    ETL_RESULT_ARITHMETICS(>=)
    ETL_RESULT_ARITHMETICS(<=)
    ETL_RESULT_ARITHMETICS(==)

#undef ETL_RESULT_ARITHMETICS
}

template <typename T, typename E>
struct std::tuple_size<Project::etl::Result<T, E>> : public Project::etl::integral_constant<size_t, 2> {};

template <size_t I, typename T, typename E>
struct std::tuple_element<I, Project::etl::Result<T, E>> { 
    using type = decltype(Project::etl::declval<Project::etl::Result<T, E>>().template get<I>()); 
};

#endif
