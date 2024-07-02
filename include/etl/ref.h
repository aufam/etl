#ifndef ETL_REF_H
#define ETL_REF_H

#include "etl/utility_basic.h"

namespace Project::etl {

    /// wrap reference and provide a reference-like interface.
    template <typename T>
    class Ref {
        T* ptr;
    
    public:
        typedef T type;

        /// empty constructor
        constexpr Ref() : ptr(nullptr) {}

        /// default constructor
        constexpr Ref(T& t) : ptr(etl::addressof(t)) {} // NOLINT

        /// default constructor
        constexpr Ref(T* t) : ptr(t) {} // NOLINT

        /// disable rvalue constructor
        Ref(T&& t) = delete;

        constexpr explicit operator bool() const { return bool(ptr); }

        /// dereference operator
        constexpr const T& operator*() const { return *ptr; }
        constexpr T& operator*() { return *ptr; }

        /// arrow operator
        constexpr const T* operator->() const { return ptr; }
        constexpr T* operator->() { return ptr; }

        /// get pointer
        constexpr const T* get() const { return ptr; }
        constexpr T* get() { return ptr; }

        /// get value if ptr is not null or return other
        constexpr const T& get_value_or(const T& other) const { return ptr ? *ptr : other; }
        constexpr T& get_value_or(T& other) { return ptr ? *ptr : other; }
    };

    template <typename T> constexpr auto
    ref() { return Ref<T>(); }

    template <typename T> constexpr auto
    ref_const() { return Ref<const T>(); }

    template <typename T> constexpr auto
    ref(T& value) { return Ref<T>(value); }

    template <typename T> constexpr auto
    ref_const(const T& value) { return Ref<const T>(value); }

    template <typename T> constexpr auto
    ref(T* value) { return Ref<T>(value); }

    template <typename T> constexpr auto
    ref_const(const T* value) { return Ref<const T>(value); }

    template <typename T> void ref(const T&&) = delete;

    template <typename T> void ref_const(const T&&) = delete;

    /// type traits
    template <typename T> struct is_ref : false_type {};
    template <typename T> struct is_ref<Ref<T>> : true_type {};
    template <typename T> struct is_ref<const Ref<T>> : true_type {};
    template <typename T> struct is_ref<volatile Ref<T>> : true_type {};
    template <typename T> struct is_ref<const volatile Ref<T>> : true_type {};
    template <typename T> inline constexpr bool is_ref_v = is_ref<T>::value;

    template <typename T> struct remove_extent<Ref<T>> { typedef T type; };
    template <typename T> struct remove_extent<const Ref<T>> { typedef T type; };
    template <typename T> struct remove_extent<volatile Ref<T>> { typedef T type; };
    template <typename T> struct remove_extent<const volatile Ref<T>> { typedef T type; };
}

#endif //ETL_REF_H
