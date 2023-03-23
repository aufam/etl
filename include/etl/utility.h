#ifndef ETL_UTILITY_H
#define ETL_UTILITY_H

#include "etl/type_traits.h"

namespace Project::etl {

    /// moves to rvalue
    template <typename T> constexpr remove_reference_t<T>&&
    move(T&& t) { return static_cast<remove_reference_t<T>&&>(t); }

    /// forward an lvalue
    template <typename T> constexpr T&&
    forward(remove_reference_t<T>& t) { return static_cast<T&&>(t); }

    /// forward an rvalue
    template <typename T> constexpr T&&
    forward(remove_reference_t<T>&& t) {
        static_assert(! is_lvalue_reference_v<T>, "Invalid rvalue to lvalue conversion");
        return static_cast<T&&>(t);
    }

    /// pointer to first element
    template <typename Container> constexpr auto
    begin(Container& cont) -> decltype(cont.begin()) { return cont.begin(); }
    template <typename Container> constexpr auto
    begin(const Container& cont) -> decltype(cont.begin()) { return cont.begin(); }
    template <typename T, size_t N> constexpr T*
    begin(T (&arr)[N]) { return arr; }

    /// pointer to one past the last element
    template <typename Container> constexpr auto
    end(Container& cont) -> decltype(cont.end()) { return cont.end(); }
    template <typename Container> constexpr auto
    end(const Container& cont) -> decltype(cont.end()) { return cont.end(); }
    template <typename T, size_t N> constexpr T*
    end(T (&arr)[N]) { return arr + N; }

    /// python-like len
    template <typename Container> constexpr auto
    len(Container& cont) -> decltype(cont.len()) { return cont.len(); }
    template <typename Container> constexpr auto
    len(const Container& cont) -> decltype(cont.len()) { return cont.len(); }
    template <typename T, size_t N> constexpr auto
    len(T (&)[N]) { return N; }

    /// python-like next
    template <typename Generator> constexpr auto
    next(Generator& gen) -> decltype(gen()) { return gen(); }
    template <typename Generator> constexpr auto
    next(const Generator& gen) -> decltype(gen()) { return gen(); }

    /// pair of values with possible different types
    template <typename X, typename Y = X>
    struct Pair {
        X x; Y y;
        constexpr bool operator==(const Pair& other) const { return x == other.x && y == other.y; }
        constexpr bool operator!=(const Pair& other) const { return !operator==(other); }
    };

    /// create pair, types are deduced
    template <typename X, typename Y> constexpr auto
    pair(X x, Y y) { return Pair<X, Y>{x, y}; }

    /// type traits
    template <typename T> struct is_pair : false_type {};
    template <typename X, typename Y> struct is_pair<Pair<X, Y>> : true_type {};
    template <typename X, typename Y> struct is_pair<const Pair<X, Y>> : true_type {};
    template <typename X, typename Y> struct is_pair<volatile Pair<X, Y>> : true_type {};
    template <typename X, typename Y> struct is_pair<const volatile Pair<X, Y>> : true_type {};
    template <typename T> inline constexpr bool is_pair_v = is_pair<T>::value;

    /// get specifier
    template <size_t i, typename X, typename Y> constexpr auto&
    get(Pair<X, Y>& p) {
        static_assert(i < 2, "index has to be less than 2");
        if constexpr (i == 0) return p.x;
        else return p.y;
    }
    template <size_t i, typename X, typename Y> constexpr auto&&
    get(Pair<X, Y>&& p) {
        static_assert(i < 2, "index has to be less than 2");
        if constexpr (i == 0) return p.x;
        else return p.y;
    }
    template <size_t i, typename X, typename Y> constexpr auto&
    get(const Pair<X, Y>& p) {
        static_assert(i < 2, "index has to be less than 2");
        if constexpr (i == 0) return p.x;
        else return p.y;
    }
    template <size_t i, typename X, typename Y> constexpr auto&&
    get(const Pair<X, Y>&& p) {
        static_assert(i < 2, "index has to be less than 2");
        if constexpr (i == 0) return p.x;
        else return p.y;
    }

    /// len specifier
    template <typename X, typename Y> constexpr size_t
    len(const Pair<X, Y>&) { return 2; }

    /// triple of values with possible different types
    template <class X, class Y = X, class Z = Y>
    struct Triple {
        X x; Y y; Z z;
        constexpr bool operator==(const Triple& other) const { return x == other.x && y == other.y && z == other.z; }
        constexpr bool operator!=(const Triple& other) const { return !operator==(other); }
    };

    /// create triple, types are deduced
    template <class X, class Y, class Z> constexpr auto
    triple(X x, Y y, Z z) { return Triple<X, Y, Z>{x, y, z}; }

    /// type traits
    template <typename T> struct is_triple : false_type {};
    template <typename X, typename Y, typename Z> struct is_triple<Triple<X, Y, Z>> : true_type {};
    template <typename X, typename Y, typename Z> struct is_triple<const Triple<X, Y, Z>> : true_type {};
    template <typename X, typename Y, typename Z> struct is_triple<volatile Triple<X, Y, Z>> : true_type {};
    template <typename X, typename Y, typename Z> struct is_triple<const volatile Triple<X, Y, Z>> : true_type {};
    template <typename T> inline constexpr bool is_triple_v = is_triple<T>::value;

    /// get specifier
    template <size_t i, class X, class Y, class Z> constexpr auto&
    get(Triple<X, Y, Z>& p) {
        static_assert(i < 2, "index has to be less than 3");
        if constexpr (i == 0) return p.x;
        if constexpr (i == 1) return p.y;
        else return p.z;
    }
    template <size_t i, class X, class Y, class Z> constexpr auto&&
    get(Triple<X, Y, Z>&& p) {
        static_assert(i < 2, "index has to be less than 3");
        if constexpr (i == 0) return p.x;
        if constexpr (i == 1) return p.y;
        else return p.z;
    }
    template <size_t i, class X, class Y, class Z> constexpr auto&
    get(const Triple<X, Y, Z>& p) {
        static_assert(i < 2, "index has to be less than 3");
        if constexpr (i == 0) return p.x;
        if constexpr (i == 1) return p.y;
        else return p.z;
    }
    template <size_t i, class X, class Y, class Z> constexpr auto&&
    get(const Triple<X, Y, Z>&& p) {
        static_assert(i < 2, "index has to be less than 3");
        if constexpr (i == 0) return p.x;
        if constexpr (i == 1) return p.y;
        else return p.z;
    }

    /// len specifier
    template <typename X, typename Y, typename Z> constexpr size_t
    len(const Triple<X, Y, Z>&) { return 3; }

    /// python-like enumerate
    template <typename Iterator>
    class Enumerate {
        Iterator first, last;
        int cnt;

    public:
        constexpr Enumerate(Iterator first, Iterator last, int cnt = 0)
        : first(first), last(last), cnt(cnt) {}

        constexpr Enumerate& begin() { return *this; }
        constexpr Enumerate& end()   { return *this; }
        constexpr bool operator!=(const Enumerate&) { return first != last; }
        constexpr void operator++()  { ++first; ++cnt; }
        constexpr auto operator*()   { return Pair<int, decltype(*first)>{cnt, *first }; }
        constexpr auto operator()()  { auto res = *(*this); ++(*this); return res; }
    };

    template <typename Iterator> constexpr auto
    enumerate(Iterator first, Iterator last, int cnt = 0) { return Enumerate(first, last, cnt); }
    template <typename Container> constexpr auto
    enumerate(Container& cont, int cnt = 0) { return Enumerate(begin(cont), end(cont), cnt); }

    /// python-like zip
    template <typename Iterator1, typename Iterator2>
    class Zip {
        Pair<Iterator1, Iterator2> first, last;

    public:
        constexpr Zip(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2)
        : first { first1, first2 }, last { last1, last2 } {}

        constexpr Zip& begin() { return *this; }
        constexpr Zip& end()   { return *this; }
        constexpr bool operator!=(const Zip&) { return first.x != last.x && first.y != last.y; }
        constexpr void operator++()   { ++first.x; ++first.y; }
        constexpr auto operator*()    { return Pair<decltype(*first.x), decltype(*first.y)> { *first.x, *first.y }; }
        constexpr auto operator()()   { auto res = *(*this); ++(*this); return res; }
    };

    template <typename Iterator1, typename Iterator2> constexpr auto
    zip(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2) { return Zip(first1, last1, first2, last2); }
    template <typename Container1, typename Container2> constexpr auto
    zip(Container1& cont1, Container2& cont2) { return Zip(begin(cont1), end(cont1), begin(cont2), end(cont2)); }

    /// python-like range
    template <typename T>
    class Range {
        T first, last, step;

    public:
        constexpr Range(T first, T last, T step) : first(first), last(last), step(step) {}

        constexpr Range& begin() { return *this; }
        constexpr Range& end()   { return *this; }
        constexpr bool operator!=(const Range&) const { return step < 0 ? first > last : first < last; }
        constexpr void operator++() { first += step; } // prefix increment
        constexpr T operator*()     { return first; }
        constexpr T operator()()    { auto res = *(*this); ++(*this); return res; }
    };

    template <typename T> constexpr enable_if_t<is_arithmetic_v<T>, Range<T>>
    range(T last) { return Range(T(0), last, T(1)); }
    template <typename T> constexpr enable_if_t<is_arithmetic_v<T>, Range<T>>
    range(T first, T last, T step = 1) { return Range(first, last, step); }

    /// python-like iter
    template <typename Iterator>
    class Iter {
        Iterator first, last;

    public:
        constexpr Iter(Iterator first, Iterator last) : first(first), last(last) {}

        constexpr Iter& begin() { return *this; }
        constexpr Iter& end()   { return *this; }
        constexpr bool operator!=(const Iter&) const { return first != last; }
        constexpr void operator++() { ++first; } // prefix increment
        constexpr auto operator*()  { return *first; }
        constexpr auto operator()() { return *first++; }

        constexpr auto& operator[](size_t i) { return *(first + i); }
        constexpr const auto& operator[](size_t i) const { return *(first + i); }

        constexpr size_t len() { return last - first; }
    };

    template <typename Iterator> constexpr auto
    iter(Iterator first, Iterator last) { return Iter(first, last); }
    template <typename Container> constexpr auto
    iter(Container& cont) { return Iter(begin(cont), end(cont)); }  
}

#endif //ETL_UTILITY_H
