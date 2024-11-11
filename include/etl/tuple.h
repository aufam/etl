#ifndef ETL_TUPLE_H
#define ETL_TUPLE_H

#include "etl/utility_basic.h"
#include <utility>

namespace Project::etl {
    /// contains the actual value for one item in the tuple.
    /// @tparam i allows the get function to find the value in O(1) time
    template <size_t i, typename T> struct TupleHead { T item; };

    /// acts as proxy for the final class that has an extra template parameter `i`.
    template <size_t i, typename... Ts> struct TupleImpl;

    /// empty tuple
    template <size_t i> struct TupleImpl<i> {};

    /// recursive specialization
    template <size_t i, typename T, typename... Ts>
    struct TupleImpl<i, T, Ts...> : public TupleHead<i, T>, public TupleImpl<i + 1, Ts...> {
        template <typename U, typename... Us>
        constexpr TupleImpl(U&& t, Us&&... ts) : TupleHead<i, T>{etl::forward<U>(t)}, TupleImpl<i + 1, Ts...>(etl::forward<Us>(ts)...) {}
    };

    /// final class, fixed size container, which holds elements of possibly different types
    template <typename... Ts> using
    Tuple = etl::TupleImpl<0, Ts...>;

    /// create tuple with variadic template function
    template <typename... Ts> constexpr auto
    tuple(Ts&&... ts) { return etl::Tuple<Ts...> { etl::forward<Ts>(ts)... }; }

    /// forward_as_tuple
    template <typename... Ts> constexpr auto
    forward_as_tuple(Ts&&... ts) noexcept { return etl::Tuple<Ts&&...> { etl::forward<Ts>(ts)... }; }

    /// pair of values with possible different types
    template <typename X, typename Y = X>
    struct Pair { X x; Y y; };

    /// create pair, types are deduced
    template <typename X, typename Y> constexpr auto
    pair(X&& x, Y&& y) { return etl::Pair<X, Y>{etl::forward<X>(x), etl::forward<Y>(y)}; }

    /// create empty pair
    template <typename X, typename Y = X> constexpr auto
    pair() { return etl::Pair<X, Y>{}; }

    /// triple of values with possible different types
    template <typename X, typename Y = X, typename Z = Y>
    struct Triple { X x; Y y; Z z; };

    /// create triple, types are deduced
    template <typename X, typename Y, typename Z> constexpr auto
    triple(X&& x, Y&& y, Z&& z) { return etl::Triple<X, Y, Z>{etl::forward<X>(x), etl::forward<Y>(y), etl::forward<Z>(z)}; }

    /// create empty triple
    template <typename X, typename Y = X, typename Z = Y> constexpr auto
    triple() { return etl::Triple<X, Y, Z>{}; }
}

/// tuple helper functions
namespace Project::etl::detail {
    template <size_t i, typename T, typename... Ts> constexpr auto&
    tuple_get(TupleImpl<i, T, Ts...>& t) { return t.TupleHead<i, T>::item; }

    template <size_t i, typename T, typename... Ts> constexpr auto&&
    tuple_get(TupleImpl<i, T, Ts...>&& t) { return etl::forward<T>(t.TupleHead<i, T>::item); }

    template <size_t i, typename T, typename... Ts> constexpr const auto&
    tuple_get(const TupleImpl<i, T, Ts...>& t) { return t.TupleHead<i, T>::item; }

    template <size_t i, typename T, typename... Ts> constexpr const auto&&
    tuple_get(const TupleImpl<i, T, Ts...>&& t) { return etl::forward<etl::add_const_t<T>>(t.TupleHead<i, T>::item); }

    template <size_t i, typename T, typename... Ts> constexpr auto&&
    tuple_get_forward(TupleImpl<i, T, Ts...>& t) { return etl::forward<T>(t.TupleHead<i, T>::item); }

    template <size_t i, typename T, typename... Ts> constexpr const auto&&
    tuple_get_forward(const TupleImpl<i, T, Ts...>& t) { return etl::forward<etl::add_const_t<T>>(t.TupleHead<i, T>::item); }

    template <size_t... i, typename T> auto
    tuple_slice(T& t, etl::integer_sequence<size_t, i...>) { return etl::tuple(etl::get<i>(t)...); }

    template <size_t... i, typename T> auto
    tuple_slice(T&& t, etl::integer_sequence<size_t, i...>) { return etl::tuple(etl::get_forward<i>(t)...); }

    template <typename T, typename U, size_t i, size_t N> constexpr bool
    tuple_eq(const T& t, const U& u) {
        if constexpr (i >= N) return true;
        else return etl::get<i>(t) == etl::get<i>(u) && etl::detail::tuple_eq<T, U, i + 1, N>(t, u);
    }

    template <typename F, typename T, size_t... i> constexpr decltype(auto)
    tuple_apply(F&& fn, T& t, etl::index_sequence<i...>) { return fn(etl::get<i>(t)...); }

    template <typename F, typename T, size_t... i> constexpr decltype(auto)
    tuple_apply(F&& fn, T&& t, etl::index_sequence<i...>) { return fn(etl::get_forward<i>(t)...); }
}

/// some trait implementations
namespace Project::etl {
    template <typename T> struct is_tuple : etl::false_type {};
    template <typename... T> struct is_tuple<etl::Tuple<T...>> : etl::true_type {};
    template <typename... T> struct is_tuple<const etl::Tuple<T...>> : etl::true_type {};
    template <typename... T> struct is_tuple<volatile etl::Tuple<T...>> : etl::true_type {};
    template <typename... T> struct is_tuple<const volatile etl::Tuple<T...>> : etl::true_type {};
    template <typename X, typename Y> struct is_tuple<etl::Pair<X, Y>> : etl::true_type {};
    template <typename X, typename Y> struct is_tuple<const etl::Pair<X, Y>> : etl::true_type {};
    template <typename X, typename Y> struct is_tuple<volatile etl::Pair<X, Y>> : etl::true_type {};
    template <typename X, typename Y> struct is_tuple<const volatile etl::Pair<X, Y>> : etl::true_type {};
    template <typename X, typename Y, typename Z> struct is_tuple<etl::Triple<X, Y, Z>> : etl::true_type {};
    template <typename X, typename Y, typename Z> struct is_tuple<const etl::Triple<X, Y, Z>> : etl::true_type {};
    template <typename X, typename Y, typename Z> struct is_tuple<volatile etl::Triple<X, Y, Z>> : etl::true_type {};
    template <typename X, typename Y, typename Z> struct is_tuple<const volatile etl::Triple<X, Y, Z>> : etl::true_type {};
    template <typename T> inline constexpr bool is_tuple_v = etl::is_tuple<T>::value;

    template <typename T> struct is_pair : etl::false_type {};
    template <typename X, typename Y> struct is_pair<etl::Pair<X, Y>> : etl::true_type {};
    template <typename X, typename Y> struct is_pair<const etl::Pair<X, Y>> : etl::true_type {};
    template <typename X, typename Y> struct is_pair<volatile etl::Pair<X, Y>> : etl::true_type {};
    template <typename X, typename Y> struct is_pair<const volatile etl::Pair<X, Y>> : etl::true_type {};
    template <typename X, typename Y> struct is_pair<etl::Tuple<X, Y>> : etl::true_type {};
    template <typename X, typename Y> struct is_pair<const etl::Tuple<X, Y>> : etl::true_type {};
    template <typename X, typename Y> struct is_pair<volatile etl::Tuple<X, Y>> : etl::true_type {};
    template <typename X, typename Y> struct is_pair<const volatile etl::Tuple<X, Y>> : etl::true_type {};
    template <typename T> inline constexpr bool is_pair_v = etl::is_pair<T>::value;

    template <typename T> struct is_triple : etl::false_type {};
    template <typename X, typename Y, typename Z> struct is_triple<etl::Triple<X, Y, Z>> : etl::true_type {};
    template <typename X, typename Y, typename Z> struct is_triple<const etl::Triple<X, Y, Z>> : etl::true_type {};
    template <typename X, typename Y, typename Z> struct is_triple<volatile etl::Triple<X, Y, Z>> : etl::true_type {};
    template <typename X, typename Y, typename Z> struct is_triple<const volatile etl::Triple<X, Y, Z>> : etl::true_type {};
    template <typename X, typename Y, typename Z> struct is_triple<etl::Tuple<X, Y, Z>> : etl::true_type {};
    template <typename X, typename Y, typename Z> struct is_triple<const etl::Tuple<X, Y, Z>> : etl::true_type {};
    template <typename X, typename Y, typename Z> struct is_triple<volatile etl::Tuple<X, Y, Z>> : etl::true_type {};
    template <typename X, typename Y, typename Z> struct is_triple<const volatile etl::Tuple<X, Y, Z>> : etl::true_type {};
    template <typename T> inline constexpr bool is_triple_v = etl::is_triple<T>::value;

    template <typename... Ts>
    struct trait_len<etl::Tuple<Ts...>> : etl::true_type {
        static constexpr size_t len(const etl::Tuple<Ts...>&) { return sizeof...(Ts); }
    };

    template <typename X, typename Y>
    struct trait_len<etl::Pair<X, Y>> : etl::true_type {
        static constexpr size_t len(const etl::Pair<X, Y>&) { return 2; }
    };

    template <typename X, typename Y, typename Z>
    struct trait_len<etl::Triple<X, Y, Z>> : etl::true_type {
        static constexpr size_t len(const etl::Triple<X, Y, Z>&) { return 3; }
    };

    template <size_t i, typename... Ts>
    struct trait_get_from_index<i, etl::Tuple<Ts...>> : etl::true_type {
        template <typename T> static constexpr decltype(auto) get(T&& t) {
            return etl::detail::tuple_get<i>(etl::forward<T>(t));
        }

        template <typename T> static constexpr decltype(auto) get_forward(T& t) {
            return etl::detail::tuple_get_forward<i>(t);
        }
    };

    template <size_t i, typename X, typename Y>
    struct trait_get_from_index<i, etl::Pair<X, Y>> : etl::true_type {
        template <typename T> static constexpr decltype(auto) get(T&& t) {
            if constexpr (i == 0) { return (t.x); }
            else if constexpr (i == 1) { return (t.y); }
            else { static_assert(etl::always_false<T>::value, "index is out of bounds"); }
        }

        template <typename T> static constexpr decltype(auto) get_forward(T& t) {
            if constexpr (i == 0) { return etl::forward<X>(t.x); }
            else if constexpr (i == 1) { return etl::forward<Y>(t.y); }
            else { static_assert(etl::always_false<T>::value, "index is out of bounds"); }
        }

        template <typename T> static constexpr decltype(auto) get_forward(const T& t) {
            if constexpr (i == 0) { return etl::forward<etl::add_const_t<X>>(t.x); }
            else if constexpr (i == 1) { return etl::forward<etl::add_const_t<Y>>(t.y); }
            else { static_assert(etl::always_false<T>::value, "index is out of bounds"); }
        }
    };

    template <size_t i, typename X, typename Y, typename Z>
    struct trait_get_from_index<i, etl::Triple<X, Y, Z>> : etl::true_type {
        template <typename T> static constexpr decltype(auto) get(T&& t) {
            if constexpr (i == 0) { return (t.x); }
            else if constexpr (i == 1) { return (t.y); }
            else if constexpr (i == 2) { return (t.z); }
            else { static_assert(etl::always_false<T>::value, "index is out of bounds"); }
        }

        template <typename T> static constexpr decltype(auto) get_forward(T& t) {
            if constexpr (i == 0) { return etl::forward<X>(t.x); }
            else if constexpr (i == 1) { return etl::forward<Y>(t.y); }
            else if constexpr (i == 2) { return etl::forward<Z>(t.z); }
            else { static_assert(etl::always_false<T>::value, "index is out of bounds"); }
        }

        template <typename T> static constexpr decltype(auto) get_forward(const T& t) {
            if constexpr (i == 0) { return etl::forward<etl::add_const_t<X>>(t.x); }
            else if constexpr (i == 1) { return etl::forward<etl::add_const_t<Y>>(t.y); }
            else if constexpr (i == 2) { return etl::forward<etl::add_const_t<Z>>(t.z); }
            else { static_assert(etl::always_false<T>::value, "index is out of bounds"); }
        }
    };

    template <size_t i, size_t j, typename T>
    struct trait_tuple_slice<i, j, T, etl::enable_if_t<etl::trait_get_from_index<i, T>::value && etl::trait_get_from_index<j-1, T>::value>>
        : etl::true_type {
        template <typename U>
        static constexpr auto get(U&& t) { return etl::detail::tuple_slice(etl::forward<U>(t), etl::make_range_sequence<i, j>{}); }
    };

    template <typename T, typename U> constexpr etl::enable_if_t<etl::is_tuple_v<T> && etl::is_tuple_v<U>, bool>
    operator==(const T& t, const U& u) {
        return etl::detail::tuple_eq<T, U, 0, std::tuple_size<etl::decay_t<T>>::value>(t, u);
    }

    /// apply a function that takes tuple as the argument
    template <typename F, typename T> constexpr decltype(auto)
    apply(F&& fn, T&& t) {
        return etl::detail::tuple_apply(etl::forward<F>(fn), etl::forward<T>(t), etl::make_index_sequence<std::tuple_size<etl::decay_t<T>>::value>{});
    }
}

/// some specializations to enable structure binding
namespace std {

    template <typename... Ts>
    struct tuple_size<Project::etl::Tuple<Ts...>> : public Project::etl::integral_constant<size_t, sizeof...(Ts)> {};

    template <size_t i, typename T, typename... Ts>
    struct tuple_element<i, Project::etl::Tuple<T, Ts...>> : tuple_element<i - 1, Project::etl::Tuple<Ts...>> {};

    template <typename T, typename... Ts>
    struct tuple_element<0, Project::etl::Tuple<T, Ts...>> { typedef T type; };

    template <size_t i>
    struct tuple_element<i, Project::etl::Tuple<>> { static_assert(i < tuple_size<Project::etl::Tuple<>>::value); };

    template <typename X, typename Y> struct tuple_size<Project::etl::Pair<X, Y>>
        : public Project::etl::integral_constant<size_t, 2> {};
    template <typename X, typename Y> struct tuple_element<0, Project::etl::Pair<X, Y>> { typedef X type; };
    template <typename X, typename Y> struct tuple_element<1, Project::etl::Pair<X, Y>> { typedef Y type; };

    template <typename X, typename Y, typename Z> struct tuple_size<Project::etl::Triple<X, Y, Z>>
        : public Project::etl::integral_constant<size_t, 3> {};
    template <typename X, typename Y, typename Z> struct tuple_element<0, Project::etl::Triple<X, Y, Z>> { typedef X type; };
    template <typename X, typename Y, typename Z> struct tuple_element<1, Project::etl::Triple<X, Y, Z>> { typedef Y type; };
    template <typename X, typename Y, typename Z> struct tuple_element<2, Project::etl::Triple<X, Y, Z>> { typedef Z type; };
}

#endif //ETL_TUPLE_H
