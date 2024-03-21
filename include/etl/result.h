#ifndef ETL_RESULT_H
#define ETL_RESULT_H

#include "etl/optional.h"

namespace Project::etl::detail {
    template <typename T, typename U>
    void result_alloc(U&& value, uint8_t* storage) {
        new (storage) T(etl::forward<U>(value));
    }

    template <typename T>
    void result_alloc(T& value, uint8_t* storage) {
        *reinterpret_cast<size_t*>(storage) = reinterpret_cast<size_t>(&value);
    }

    template <typename T>
    decltype(auto) result_deref(uint8_t* storage) {
        if constexpr (etl::is_reference_v<T>) {
            size_t addr = *reinterpret_cast<size_t*>(storage);
            return *reinterpret_cast<etl::add_pointer_t<etl::remove_reference_t<T>>>(addr);
        } else {
            return etl::move(*reinterpret_cast<etl::add_pointer_t<T>>(storage));
        }
    }

    template <typename T>
    decltype(auto) result_deref(const uint8_t* storage) {
        if constexpr (etl::is_reference_v<T>) {
            size_t addr = *reinterpret_cast<const size_t*>(storage);
            return *reinterpret_cast<etl::add_pointer_t<etl::add_const_t<etl::remove_reference_t<T>>>>(addr);
        } else {
            return *reinterpret_cast<etl::add_pointer_t<etl::add_const_t<T>>>(storage);
        }
    }
}

namespace Project::etl {
    /// Represents a variant type that holds an ok value. the type can be void
    /// @tparam T The type of the ok value or void if not specified.
    template <typename... T>
    class Ok {
        static_assert(sizeof...(T) <= 1, "Ok can hold at most one type.");

    public:
        using type = typename std::tuple_element<sizeof...(T), std::tuple<uint8_t, T...>>::type;
        type data;

        constexpr Ok(T... arg) : data(etl::forward<T>(arg)...) {}
    };

    /// Represents a variant type that holds an error value of a specified type.
    /// @tparam E The type of the error value.
    template <typename E>
    class Err {
        static_assert(!etl::is_void_v<E>, "Error variant can't be void");

    public:
        using type = E;
        type data;

        constexpr Err(E data) : data(etl::forward<E>(data)) {}
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
        Result(Ok<U> value) {
            detail::result_alloc<T>(etl::forward<U>(value.data), storage);
            flag = 1;
        }

        /// Constructs a Result object from an Err variant.
        template <typename U, typename = enable_if_t<etl::is_convertible_v<U, E>>>
        Result(Err<U> err) {
            detail::result_alloc<E>(etl::forward<U>(err.data), storage);
            flag = 2;
        }

        Result(const Result&) = delete;

        Result(Result&& other) {
            flag = etl::exchange(other.flag, 0);
            etl::copy(other.storage, storage);
        }

        Result(Result& other) {
            flag = etl::exchange(other.flag, 0);
            etl::copy(other.storage, storage);
        }

        Result& operator=(const Result&) = delete;

        ~Result() {
            auto ptr = reinterpret_cast<void*>(storage);
            if constexpr (!etl::is_reference_v<T>) {
                if (flag == 1) {
                    reinterpret_cast<T*>(ptr)->~T();
                }    
            } 
            if constexpr (!etl::is_reference_v<E>) {
                if (flag == 2) {
                    reinterpret_cast<E*>(ptr)->~E();
                }
            }
            flag = 0;
        }

        /// Unwraps the value contained in Ok variant.
        template <typename U = T, typename = etl::enable_if_t<etl::has_empty_constructor_v<U>>>
        U unwrap() {
            return flag == 1 ? U(detail::result_deref<T>(storage)) : U();
        }
        
        /// Unwraps the value contained in Ok variant or returns a default value.
        template <typename U>
        U unwrap_or(U default_value) {
            return flag == 1 ? U(detail::result_deref<T>(storage)) : etl::forward<U>(default_value);
        }

        /// Unwraps the value contained in Err variant.
        template <typename U = E, typename = etl::enable_if_t<etl::has_empty_constructor_v<E>>>
        U unwrap_err() {
            return flag == 2 ? U(detail::result_deref<E>(storage)) : U();
        }
        
        /// Unwraps the value contained in Err variant or returns a default value.
        template <typename U>
        U unwrap_err_or(U default_value) {
            return flag == 2 ? U(detail::result_deref<E>(storage)) : etl::forward<U>(default_value);
        }

        /// structured binding support
        template <size_t I>
        auto get() {
            if constexpr (I == 0) {
                return flag == 1 ? Optional<T>(detail::result_deref<T>(storage)) : Optional<T>();
            } else if constexpr (I == 1) {
                return flag == 2 ? Optional<E>(detail::result_deref<E>(storage)) : Optional<E>();
            }
        }

        /// Chains a function call on Ok variant.
        template<typename F, typename R = decltype(etl::declval<F>()(etl::declval<T>()))>
        Result<R, E> then(F&& fn) {
            switch (flag) {
                case 1: 
                    if constexpr (etl::is_void_v<R>) return (fn(detail::result_deref<T>(storage)), Ok());
                    else return Ok(fn(detail::result_deref<T>(storage)));
                case 2: return Err(detail::result_deref<E>(storage));
                default: return {};
            }
        }

        /// Chains a function call on Err variant.
        template<typename F, typename R = decltype(etl::declval<F>()(etl::declval<E>()))>
        Result<T, R> except(F&& fn) {
            switch (flag) {
                case 2: return Err(fn(detail::result_deref<E>(storage)));
                case 1: return Ok(detail::result_deref<T>(storage));
                default: return {};
            }
        }

    private:
        Result() {}

        template <typename TT, typename EE>
        friend class Result;

        using Biggest = etl::conditional_t<(sizeof(T) > sizeof(E)), T, E>;
        alignas(T) uint8_t storage[((sizeof(Biggest)) / sizeof(size_t)) * sizeof(size_t) + sizeof(size_t)] = {}; // ceil to 4 or 8 bytes
        mutable size_t flag = 0;
    };

    template <typename E>
    class Result<void, E> {
    public:
        using value_type = void;
        using error_type = E;

        Result(const Ok<>&) {
            flag = 1;
        }

        template <typename U>
        Result(Err<U> err) {
            detail::result_alloc<E>(etl::forward<U>(err.data), storage);
            flag = 2;
        }

        Result(const Result&) = delete;
        
        Result(Result&& other) {
            flag = etl::exchange(other.flag, 0);
            etl::copy(other.storage, storage);
        }

        template <typename T>
        Result(Result<T, E>& other) {
            if (other.flag == 2) {
                flag = etl::exchange(other.flag, 0);
                etl::copy(other.storage, storage);
            }
        }

        template <typename T>
        Result(Result<T, E>&& other) {
            if (other.flag == 2) {
                flag = etl::exchange(other.flag, 0);
                etl::copy(other.storage, storage);
            }
        }

        Result& operator=(const Result&) = delete;

        ~Result() {
            auto ptr = reinterpret_cast<void*>(storage);
            if constexpr (!etl::is_reference_v<E>) {
                if (flag == 2) {
                    reinterpret_cast<E*>(ptr)->~E();
                }
            }
            flag = 0;
        }

        void unwrap() {
            return;
        }

        template <typename U = E, typename = etl::enable_if_t<etl::has_empty_constructor_v<E>>>
        U unwrap_err() {
            return flag == 2 ? U(detail::result_deref<E>(storage)) : U();
        }
        
        template <typename U>
        U unwrap_err(U default_value) {
            return flag == 2 ? U(detail::result_deref<E>(storage)) : etl::forward<U>(default_value);
        }
        
        template <size_t I>
        auto get() {
            if constexpr (I == 0) {
                return flag == 1;
            } else if constexpr (I == 1) {
                return flag == 2 ? Optional<E>(detail::result_deref<E>(storage)) : Optional<E>();
            }
        }

        template<typename F, typename R = decltype(etl::declval<F>()())>
        Result<R, E> then(F&& fn) {
            switch (flag) {
                case 1: 
                    if constexpr (etl::is_void_v<R>) return (fn(), Ok());
                    else return Ok(fn());
                case 2: return Err(detail::result_deref<E>(storage));
                default: return {};
            }
        }

        template<typename F, typename R = decltype(etl::declval<F>()(etl::declval<E>()))>
        Result<void, R> except(F&& fn) {
            switch (flag) {
                case 2: return Err(fn(detail::result_deref<E>(storage)));
                case 1: return Ok();
                default: return {};
            }
        }

    private:
        Result() {}

        template <typename TT, typename EE>
        friend class Result;

        alignas(E) uint8_t storage[(sizeof(E) / sizeof(size_t)) * sizeof(size_t) + sizeof(size_t)] = {};
        mutable size_t flag = 0;
    };
}

template <typename T, typename E>
struct std::tuple_size<Project::etl::Result<T, E>> : public Project::etl::integral_constant<size_t, 2> {};

template <size_t i, typename T, typename E>
struct std::tuple_element<i, Project::etl::Result<T, E>> { 
    using type = decltype(Project::etl::declval<Project::etl::Result<T, E>>().template get<i>()); 
};

#endif
