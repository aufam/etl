#ifndef ETL_ANY_H
#define ETL_ANY_H

#include "etl/allocator.h"

namespace Project::etl {

    /// provide a dynamic storage for storing and manipulating value of any type
    /// @note if the type has custom destructor, you have to manually delete it by invoking detach<T>() 
    /// before destructing or reassigning this object
    class Any {
        etl::Allocator<uint8_t> alloc;
        uint8_t* ptr = nullptr;
        size_t n = 0;
        void (*copy_construct)(uint8_t*, uint8_t*) = nullptr;
        void (*copy_assign)(uint8_t*, uint8_t*) = nullptr;
        void (*destruct)(uint8_t*) = nullptr;

    public:
        /// empty constructor
        constexpr Any() {}

        /// copy constructors
        Any(const Any& other) { 
            if (other.copy_construct == nullptr)
                return;
            
            alloc = other.alloc;
            ptr = alloc.allocate(other.n);
            if (ptr == nullptr)
                return;
            
            other.copy_construct(other.ptr, ptr);
            n = other.n;
            copy_construct = other.copy_construct;
            copy_assign = other.copy_assign;
            destruct = other.destruct;
        }

        /// copy assignment
        Any& operator=(const Any& other) {
            if (&other == this) return *this;

            if (is_same_type(other)) {
                if (copy_assign) copy_assign(other.ptr, ptr);
                return *this;
            }

            Any temp = other;
            *this = etl::move(temp);
            return *this;
        }

        /// move constructor
        Any(Any&& other) noexcept { 
            *this = etl::move(other);
        }

        /// move assignment
        Any& operator=(Any&& other) noexcept {
            if (&other == this) return *this;

            detach();
            alloc = etl::move(other.alloc);
            ptr = etl::exchange(other.ptr, nullptr);
            n = etl::exchange(other.n, 0);
            copy_construct = etl::exchange(other.copy_construct, nullptr);
            copy_assign = etl::exchange(other.copy_assign, nullptr);
            destruct = etl::exchange(other.destruct, nullptr);
            return *this;
        }

        /// construct from any type
        /// @note disable if value type is Any& or const T&&
        template <typename T, typename = disable_if_t<is_same_v<T, Any&> || is_const_v<T>>>
        Any(T&& value) {
            using U = decay_t<T>;
            if constexpr (!is_same_v<U, None>) {
                ptr = alloc.allocate(sizeof(U));
                if (ptr == nullptr) 
                    return;
                
                new(ptr) U(etl::forward<T>(value));
                n = sizeof(U);

                if constexpr (etl::is_copy_constructible_v<U>)
                copy_construct = +[] (uint8_t* src, uint8_t* dest) { new(dest) U(*reinterpret_cast<U*>(src)); };
                
                if constexpr (etl::is_copy_assignable_v<U>)
                copy_assign = +[] (uint8_t* src, uint8_t* dest) { *reinterpret_cast<U*>(dest) = *reinterpret_cast<U*>(src); };

                if constexpr (etl::is_destructible_v<U>)
                destruct = +[] (uint8_t* ptr) { reinterpret_cast<U*>(ptr)->~U(); };
            }
        }

        /// assign from any type
        /// @note disable if value type is Any& or const T&&
        template <typename T, typename = disable_if_t<is_same_v<T, Any&> || is_const_v<T>>>
        Any& operator=(T&& value) {
            Any other = etl::forward<T>(value);
            *this = etl::move(other);
            return *this;
        }

        ~Any() { detach(); }

        size_t size() const { return n; }

        /// invoke destructor T and reset
        void detach() {
            if (ptr == nullptr) return;
            if (destruct) destruct(ptr);
            
            alloc.deallocate(ptr, n);
            ptr = nullptr;
            n = 0;
            copy_construct = nullptr;
            copy_assign = nullptr;
            destruct = nullptr;
        }

        bool is_same_type(const Any& other) const {
            return copy_construct == other.copy_construct && copy_assign == other.copy_assign && destruct == other.destruct;
        }

        /// cast to any type
        /// @note the type has to be the last type this object is assigned to
        template <typename T> const T& as() const { return *reinterpret_cast<const T*>(ptr); }
        template <typename T> T& as() { return *reinterpret_cast<T*>(ptr); }

        /// get raw pointer
        const void* get() const { return ptr; }
        void* get() { return ptr; }

        template <typename T> const T& get_value_or(const T& other) const { return ptr ? as<T>() : other; }
        template <typename T> T& get_value_or(T& other) const { return ptr ? as<T>() : other; }

        explicit operator bool() const { return bool(ptr); }
    };

    /// create empty any object
    inline auto any() { return etl::Any(); }

    /// create any object of a given value
    template <typename T> auto
    any(T&& value) { return etl::Any(etl::forward<T>(value)); }
}

#endif // ETL_ANY_H