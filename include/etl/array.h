#ifndef ETL_ARRAY_H
#define ETL_ARRAY_H

#include "etl/algorithm.h"
#include <cstring> // memcpy

namespace Project::etl {

    template <typename T, size_t N>
    struct array_traits {
        typedef T type[N];
        static constexpr T* ptr(const type& buf) noexcept { return const_cast<T*>(buf); }
        static constexpr T& ref(const type& buf, int i) noexcept {
            if (i < 0) i = N + i; // allowing negative index
            return const_cast<T&>(buf[i]);
        }
    };

    template <typename T>
    struct array_traits<T, 0> {
        typedef etl::None type;
        static constexpr T* ptr(const type&) noexcept { return nullptr; }
        static constexpr T& ref(const type&, int) noexcept { return *static_cast<T*>(nullptr); }
    };

    template <typename T, size_t N> using array_traits_t = typename array_traits<T, N>::type;

    /// static contiguous array
    template <typename T, size_t N>
    struct Array {
        typedef T value_type;
        typedef array_traits<T, N> traits;
        typedef array_traits_t<T, N> traits_type;
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef T& reference;
        typedef const T& const_reference;

        traits_type buf;

        [[nodiscard]] static constexpr size_t size() { return N; }
        [[nodiscard]] constexpr size_t len() const { return N; }
        constexpr explicit operator bool() const { return N > 0; }
        constexpr bool empty() const { return N == 0; }

        constexpr iterator data()   { return traits::ptr(buf); }
        constexpr iterator begin()  { return (iterator)(data()); }
        constexpr iterator end()    { return (iterator)(data() + N); }
        constexpr reference front() { return *begin(); }
        constexpr reference back()  { return N ? *(end() - 1) : *end(); }

        constexpr const_iterator data()   const { return traits::ptr(buf); }
        constexpr const_iterator begin()  const { return (const_iterator)(data()); }
        constexpr const_iterator end()    const { return (const_iterator)(data() + N); }
        constexpr const_reference front() const { return traits::ref(buf, 0); }
        constexpr const_reference back()  const { return traits::ref(buf, N - 1); }

        constexpr reference operator[](int i) { return traits::ref(buf, i); }
        constexpr const_reference operator[](int i) const { return traits::ref(buf, i); }

        /// slice operator
        constexpr Iter<iterator> operator()(int start, int stop, int step = 1) {
            return start < stop ? etl::iter(&operator[](start), &operator[](stop), step) : etl::iter(begin(), begin(), step);
        }

        constexpr Iter<const_iterator> operator()(int start, int stop, int step = 1) const {
            return start < stop ? etl::iter(&operator[](start), &operator[](stop), step) : etl::iter(begin(), begin(), step);
        }

        constexpr Iter<iterator> iter() { return Iter(begin(), end(), 1); }
        constexpr Iter<const_iterator> iter() const { return Iter(begin(), end(), 1); }

        constexpr Iter<iterator> reversed() { return Iter(end() - 1, begin() - 1, -1); }
        constexpr Iter<const_iterator> reversed() const { return Iter(end() - 1, begin() - 1, -1); }
    };

    /// create array with variadic template function, array type can be explicitly or implicitly specified, array size is implicitly specified
    template <typename T = void, typename U, typename... Us, typename R = conditional_t<is_void_v<T>, decay_t<U>, T>> constexpr auto
    array(U&& val, Us&&... vals) { return Array<R, 1 + sizeof...(Us)> { R(etl::forward<U>(val)), R(etl::forward<Us>(vals))... }; }

    /// create array from initializer list
    template <typename T, size_t N> constexpr Array<T, N>
    array(std::initializer_list<T> items) { Array<T, N> res; etl::copy(items.begin(), items.end(), res.begin(), res.end()); return res; }

    /// create array with default constructed T, array type and size are explicitly specified
    template <typename T, size_t N> constexpr auto
    array() { return Array<T, N> {}; }

    /// cast reference from any pointer
    template <typename T, size_t N, typename U> constexpr auto&
    array_cast(U* a) { return *reinterpret_cast<conditional_t<is_const_v<U>, const Array<T, N>*, Array<T, N>*>>(a); }

    /// cast reference from any type
    template <typename T = void, typename U, typename R = conditional_t<is_void_v<T>, remove_extent_t<U>, T>> constexpr auto&
    array_cast(U& a) { return etl::array_cast<R, sizeof(U) / sizeof(R)>(&a); }

    /// swap specialization, avoid creating large temporary variable
    template <typename T, typename U, typename = enable_if_t<is_same_v<remove_extent_t<T>, remove_extent_t<U>>>> constexpr void
    swap(T& a, U& b) { etl::swap_element(a, b); }
}

#endif //ETL_ARRAY_H
