#ifndef ETL_OPTIONAL_H
#define ETL_OPTIONAL_H

#include "etl/utility_basic.h"

namespace Project::etl {

    template <typename T>
    class optional {
        bool initialized;
        T value;

    public:
        typedef T type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef T&& rval_reference;

        /// empty constructor
        constexpr optional() : initialized(false), value() {}

        /// construct from None type
        constexpr optional(None_t) : initialized(false), value() {}

        /// copy construct from original type
        constexpr optional(const_reference value) : optional(true, value) {}

        /// move construct from original type
        constexpr optional(rval_reference value) : optional(true, etl::move(value)) {}

        /// copy construct from another optional<T>
        optional(const optional&) = default;

        /// move construct from another optional<T>
        optional(optional&&) = default;

        /// copy construct from another convertible optional<U>
        template <typename U> explicit
        constexpr optional(const optional<U>& other) : optional(other.initialized, static_cast<T>(other.value)) {}

        /// move construct from another convertible optional<U>
        template <typename U> explicit
        constexpr optional(optional<U>&& other) : optional(other.initialized, static_cast<T>(etl::move(other.value))) {}

        /// move/copy construct from another convertible type
        template <typename... Args> explicit
        constexpr optional(Args&&... args) : optional(true, T(etl::forward<Args>(args)...)) {}

        /// copy assign from original type
        constexpr optional& operator=(const_reference other) { return assign(bool(etl::addressof(other)), other); }

        /// move assign from original type
        constexpr optional& operator=(rval_reference other) { return assign(bool(etl::addressof(other)), etl::move(other)); }

        /// copy assign from another optional<T>
        optional& operator=(const optional&) = default;

        /// move assign from another optional<T>
        optional& operator=(optional&&) = default;

        /// copy assign from another convertible optional<U>
        template <typename U>
        constexpr optional& operator=(const optional<U>& other) { return assign(other.initialized, static_cast<T>(other.value)); }

        /// move assign from another convertible optional<U>
        template <typename U>
        constexpr optional& operator=(optional<U>&& other) { return assign(other.initialized, static_cast<T>(etl::move(other))); }

        /// move/copy assign from another convertible type U
        template <typename U>
        constexpr optional& operator=(U&& other) { return assign(bool(etl::addressof(other)), T(etl::forward<U>(other))); }

        /// assign none type
        constexpr optional& operator=(None_t) { initialized = false; return *this; }
        
        /// check if the value is valid
        constexpr bool is_initialized() const { return initialized; }
        constexpr explicit operator bool() const { return initialized; }

        /// arrow operator
        constexpr const_pointer operator->() const { return &value; }
        constexpr pointer operator->() { return &value; }

        /// dereference operator
        constexpr const_reference operator*() const& { return value; }
        constexpr reference operator*() & { return value; }

        /// return value if initialized or return other
        constexpr const_reference get_value_or(const_reference other) const { return initialized ? value : other; }
        constexpr reference get_value_or(reference other) { return initialized ? value : other; }

    private:
        /// default copy constructor helper
        constexpr optional(bool cond, const_reference value) : initialized(cond), value(value) {}

        /// default move constructor helper
        constexpr optional(bool cond, rval_reference value) : initialized(cond), value(etl::move(value)) {}

        /// copy assign helper
        constexpr optional& assign(bool cond, const_reference other) {
            initialized = cond;
            if (initialized) value = other;
            return *this;
        }

        /// move assign helper
        constexpr optional& assign(bool cond, rval_reference other) {
            initialized = cond;
            if (initialized) value = etl::move(other);
            return *this;
        }
    };
}

#endif // ETL_OPTIONAL_H