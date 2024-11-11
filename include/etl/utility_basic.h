#ifndef ETL_UTILITY_BASIC_H
#define ETL_UTILITY_BASIC_H

#include "etl/type_traits.h"

namespace Project::etl {

    /// moves to rvalue
    template <typename T> constexpr etl::remove_reference_t<T>&&
    move(T&& t) { return static_cast<etl::remove_reference_t<T>&&>(t); }

    /// forward an lvalue
    template <typename T> constexpr T&&
    forward(etl::remove_reference_t<T>& t) { return static_cast<T&&>(t); }

    /// forward an rvalue
    template <typename T> constexpr T&&
    forward(etl::remove_reference_t<T>&& t) {
        static_assert(! etl::is_lvalue_reference_v<T>, "Invalid rvalue to lvalue conversion");
        return static_cast<T&&>(t);
    }

    /// exchange the values of a and b
    template <class T> constexpr void
    swap(T& a, T& b) {
        T temp(etl::move(a));
        a = etl::move(b);
        b = etl::move(temp);
    }

    /// replace the value of obj with new_value and returns the old value of obj
    template <typename T, typename U = T> constexpr T
    exchange(T& obj, U&& new_value) {
        T old = etl::move(obj);
        obj = etl::forward<U>(new_value);
        return old;
    }

    /// python-like len
    template <typename T> constexpr auto
    len(T&& t) { return etl::trait_len<etl::decay_t<T>>::len(etl::forward<T>(t)); }

    template <typename T, size_t N> constexpr size_t
    len(T (&)[N]) { return N; }

    /// pointer to first element
    template <typename Container> constexpr auto
    begin(Container&& cont) -> decltype(cont.begin()) { return cont.begin(); }

    template <typename T, size_t N> constexpr T*
    begin(T (&arr)[N]) { return arr; }

    /// pointer to one past the last element
    template <typename Container> constexpr auto
    end(Container&& cont) -> decltype(cont.end()) { return cont.end(); }

    template <typename T, size_t N> constexpr T*
    end(T (&arr)[N]) { return arr + N; }

    /// get the i-th element
    template <size_t i, typename T> constexpr decltype(auto)
    get(T&& t) { return etl::trait_get_from_index<i, etl::decay_t<T>>::get(etl::forward<T>(t)); }

    /// get the i-th element
    template <typename I, typename T> constexpr decltype(auto)
    get(T&& t) { return etl::trait_get_from_type<I, etl::decay_t<T>>::get(etl::forward<T>(t)); }

    /// tuple slice
    template <size_t i, size_t j, typename T> constexpr decltype(auto)
    get(T&& t) { return etl::trait_tuple_slice<i, j, etl::decay_t<T>>::get(etl::forward<T>(t)); }

    template <size_t i, typename T> constexpr decltype(auto)
    get_forward(T& t) { return etl::trait_get_from_index<i, etl::decay_t<T>>::get_forward(t); }

    template <typename I, typename T> constexpr decltype(auto)
    get_forward(T& t) { return etl::trait_get_from_type<I, etl::decay_t<T>>::get_forward(t); }

    /// python-like next
    template <typename T> constexpr decltype(auto)
    next(T&& t) { return etl::trait_generator<etl::decay_t<T>>::next(t); }

    /// python-like iter
    template <typename T> constexpr decltype(auto)
    iter(T&& t) { return etl::trait_iterable<etl::decay_t<T>>::iter(t); }

    /// python-like reversed
    template <typename T> constexpr decltype(auto)
    reversed(T&& t) { return etl::trait_reverse_iterable<etl::decay_t<T>>::reversed(t); }

    /// get the address of an object
    template <typename T> constexpr T*
    addressof(T& x) { return &x; }

    /// constant rvalue reference does not have an address
    template<class T> void
    addressof(const T&&) = delete;

    /// none type
    inline static constexpr struct None {
        template <typename T> 
        static constexpr bool is_valid_type_v = (etl::is_iterator_v<T> && etl::has_operator_bool_v<T>);

        constexpr bool operator==(None) const { return true; }
        constexpr bool operator!=(None) const { return false; }
    } none;

    template <typename T> constexpr etl::enable_if_t<etl::None::is_valid_type_v<T>, bool>
    operator==(const T& value, None) { return !bool(value); }

    template <typename T> constexpr etl::enable_if_t<etl::None::is_valid_type_v<T>, bool>
    operator==(None, const T& value) { return operator==(value, etl::none); }

    template <typename T> constexpr etl::enable_if_t<etl::None::is_valid_type_v<T>, bool>
    operator!=(const T& value, None) { return !operator==(value, etl::none); }

    template <typename T> constexpr etl::enable_if_t<etl::None::is_valid_type_v<T>, bool>
    operator!=(None, const T& value) { return !operator==(value, etl::none); }

    inline static constexpr struct {
        template <typename T> constexpr void operator=(T&&) const {}
    } ignore;

    inline static constexpr struct {
        template <typename T> constexpr auto operator|(T&& o) const { return etl::move(o); }
    } mv;

    inline static constexpr struct {
        template <typename T> constexpr auto operator|(T&& o) const { return o; }
    } cp;

    inline static constexpr struct {
        template <typename T> auto operator|(T&& o) const { return o.await(); }
    } await;

    /// manages cleanup actions on scope exit using RAII principles.
    template <typename F>
    class [[nodiscard]] Defer {
        F on_exit;
        bool enabled;

    public:
        Defer(F&& fn) : on_exit(etl::forward<F>(fn)), enabled(true) {}
        ~Defer() { if (enabled) on_exit(); }

        void disable() { enabled = false; }
        void enable() { enabled = true; }

        Defer(const Defer&) = delete;             // No copy constructor
        Defer& operator=(const Defer&) = delete;  // No copy assignment

        Defer(Defer&&) = delete;                  // No move constructor
        Defer& operator=(Defer&&) = delete;       // No move assignment
    };

    inline static constexpr struct {
        template <typename F> auto operator|(F&& fn) const { return Defer<etl::decay_t<F>>(etl::forward<F>(fn)); }
    } defer;

    inline static constexpr struct Wildcard {
        template <typename T> constexpr void operator=(T&&) const {}
    } _;

    template <typename T> constexpr bool operator==(Wildcard, T&&) { return true; }
    template <typename T> constexpr bool operator==(T&&, Wildcard) { return true; }
    template <typename T> constexpr bool operator!=(Wildcard, T&&) { return false; }
    template <typename T> constexpr bool operator!=(T&&, Wildcard) { return false; }

    // TODO
    template <typename T, typename U> constexpr etl::enable_if_t<
        etl::trait_iterable<T>::value && etl::trait_iterable<U>::value
    , bool> operator==(const T& t, const U& u) {
        if constexpr (etl::trait_len<T>::value && etl::trait_len<U>::value) {
            auto length = etl::len(t);
            if (length != etl::len(u)) return false;
        }

        auto a = etl::iter(t);
        auto b = etl::iter(u);
        for (; bool(a) && bool(b); ++a, ++b) if (*a != *b) return false;
        return true;
    }
}


/// trait default specializations
namespace Project::etl {
    template <typename T> struct trait_len<T, etl::enable_if_t<etl::detail::trait_has_len<T>::value>> : etl::true_type {
        static constexpr auto len(const T& t) { return t.len(); }
    };

    template <typename T> struct trait_len<T, etl::enable_if_t<
        !etl::detail::trait_has_len<T>::value && etl::detail::trait_has_size<T>::value
    >> : etl::true_type {
        static constexpr auto len(const T& t) { return t.size(); }
    };

    template <size_t i, typename T> struct trait_get_from_index<i, T, decltype(void(etl::declval<T>()[0]))> : etl::true_type {
        static constexpr decltype(auto) get(T& t) { return t[i]; }
        static constexpr decltype(auto) get(T&& t) { return etl::move(t[i]); }
        static constexpr decltype(auto) get_forward(T& t) { return etl::move(t[i]); }
    };

    template <typename T> struct trait_generator<T, decltype(void(etl::declval<T>()()))> : etl::true_type {
        template <typename U> static constexpr decltype(auto) next(U& t) { return t(); }
    };

    template <typename T> struct trait_iterable<T, etl::enable_if_t<etl::detail::trait_has_iter<T>::value>> : etl::true_type {
        template <typename U> static constexpr decltype(auto) iter(U& t) { return t.iter(); }
    };

    template <typename T> struct trait_reverse_iterable<T, etl::enable_if_t<etl::detail::trait_has_reversed<T>::value>> : etl::true_type {
        template <typename U> static constexpr decltype(auto) reversed(U& t) { return t.reversed(); }
    };
}

#endif // ETL_UTILITY_BASIC_H
