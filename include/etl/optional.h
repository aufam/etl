#ifndef ETL_OPTIONAL_H
#define ETL_OPTIONAL_H

#include "etl/algorithm.h"

namespace Project::etl {

    /// provide a way to represent an optional value that may or may not be present
    template <typename T>
    class Optional {
        alignas(T) uint8_t storage[sizeof(T)];
        bool valid;

    public:
        typedef T type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef T&& rval_reference;

        /// empty constructor
        constexpr Optional() : storage{}, valid(false) {}

        /// copy construct from original type
        constexpr explicit Optional(const_reference value) : storage{}, valid(bool(etl::addressof(value))) {
            if (valid) { new (storage) T(value); }
        }

        /// move construct from original type
        constexpr explicit Optional(rval_reference value) : storage{}, valid(bool(etl::addressof(value))) {
            if (valid) { new (storage) T(etl::move(value)); }
        }

#if __cplusplus > 201703L
        constexpr
#endif
        ~Optional() {
            if (valid) {
                reinterpret_cast<T*>(storage)->~T();
                valid = false;
            }
        }

        /// copy construct from another Optional<T>
        constexpr Optional(const Optional& other) : storage{}, valid(other.valid) {
            if (valid) { new (storage) T(*other); }
        }

        /// move construct from another Optional<T>
        constexpr Optional(Optional&& other) noexcept : storage{}, valid(etl::exchange(other.valid, false)) {
            etl::copy(other.storage, storage);
        }

        /// copy assign from another Optional<T>
        constexpr Optional& operator=(const Optional& other) {
            if (this == &other) {
                return *this;
            }

            if (valid) {
                *reinterpret_cast<T*>(storage) = *other;
            } else {
                new (storage) T(*other);
                valid = true;
            }

            return *this;
        }

        /// move assign from another Optional<T>
        constexpr Optional& operator=(Optional&& other) noexcept {
            if (this == &other) {
                return *this;
            }

            if (valid) {
                *reinterpret_cast<T*>(storage) = etl::move(*other);
            } else {
                new (storage) T(etl::move(*other));
                valid = true;
            }

            return *this;
        }

        /// move/copy assign from another convertible type U
        template <typename U>
        constexpr Optional& operator=(U&& other) { 
            if (!bool(etl::addressof(other))) {
                this->~Optional();
                return *this;
            }

            if (valid) {
                *reinterpret_cast<T*>(storage) = etl::forward<U>(other);
            } else {
                new (storage) T(etl::forward<U>(other));
                valid = true;
            }

            return *this;
         }

        /// assign none type
        constexpr Optional& operator=(None) { 
            this->~Optional(); 
            return *this; 
        }
        
        /// check if the value is valid
        constexpr explicit operator bool() const { return valid; }

        /// arrow operator
        constexpr const_pointer operator->() const { return valid ? reinterpret_cast<const T*>(&storage[0]) : nullptr; }
        constexpr pointer operator->() { return valid ? reinterpret_cast<T*>(&storage[0]) : nullptr; }

        /// dereference operator
        constexpr const_reference operator*() const { return *reinterpret_cast<const T*>(valid ? &storage[0] : nullptr); }
        constexpr reference operator*() { return *reinterpret_cast<T*>(valid ? &storage[0] : nullptr); }

        /// get pointer
        constexpr const_pointer get() const { return valid ? reinterpret_cast<const T*>(&storage[0]) : nullptr; }
        constexpr pointer get() { return valid ? reinterpret_cast<T*>(&storage[0]) : nullptr; }

        /// return value if valid or return other
        constexpr const_reference get_value_or(const_reference other) const { return valid ? *(*this) : other; }
        constexpr reference get_value_or(reference other) { return valid ? *(*this) : other; }
    };

    // specialization for lvalue reference
    template <typename T>
    class Optional<T&> {
        T* ptr;
    
    public:
        constexpr Optional() : ptr(nullptr) {}

        constexpr Optional(T& value) : ptr(&value) {}

        /// assign none type
        constexpr Optional& operator=(None) { 
            ptr = nullptr; 
            return *this; 
        }

        /// check if the value is valid
        constexpr explicit operator bool() const { return ptr; }

        /// arrow operator
        constexpr T* operator->() const { return ptr; }

        /// dereference operator
        constexpr T& operator*() const { return *ptr; }

        /// get pointer
        constexpr T* get() const { return ptr; }

        /// return value if valid or return other
        constexpr T& get_value_or(T& other) const { return ptr ? *ptr : other; }
    };
    
    /// create optional the type can be explicitly or implicitly specified
    template <typename T> constexpr auto
    optional(T&& value) { return Optional<T>(etl::forward<T>(value)); }

    /// create optional with variadic template function, the type has to be implicitly specified
    template <typename T, typename... Args> constexpr auto
    optional() { return Optional<T>(); }

    /// type traits
    template <typename T> struct is_optional : false_type {};
    template <typename T> struct is_optional<Optional<T>> : true_type {};
    template <typename T> struct is_optional<const Optional<T>> : true_type {};
    template <typename T> struct is_optional<volatile Optional<T>> : true_type {};
    template <typename T> struct is_optional<const volatile Optional<T>> : true_type {};
    template <typename T> inline constexpr bool is_optional_v = is_optional<T>::value;

    template <typename T> struct remove_extent<Optional<T>> { typedef T type; };
    template <typename T> struct remove_extent<const Optional<T>> { typedef T type; };
    template <typename T> struct remove_extent<volatile Optional<T>> { typedef T type; };
    template <typename T> struct remove_extent<const volatile Optional<T>> { typedef T type; };
}

#endif // ETL_OPTIONAL_H
