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

    /// create empty pair
    template <typename X, typename Y = X> constexpr auto
    pair() { return Pair<X, Y>{}; }

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

    /// create empty triple
    template <class X, class Y = X, class Z = Y> constexpr auto
    triple() { return Triple<X, Y, Z>{}; }

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

    /// python-like iter
    template <typename Iterator>
    class Iter {
        Iterator first, last;
        int step;

    public:
        constexpr Iter(Iterator first, Iterator last, int step = 1) : first(first), last(last), step(step) {}

        [[nodiscard]] constexpr size_t len() const {
            return step == 0 ? 0 : step > 0 ? (last - first) / step : (first - last) / (-step);
        }

        constexpr Iter begin() const { return *this; }
        constexpr Iter end()   const { return *this; }

        constexpr auto& operator[](int i) { 
            if (i < 0) i = len() + i; // allowing negative index
            return *(first + (step * i)); 
        }
        constexpr const auto& operator[](int i) const { 
            if (i < 0) i = len() + i; // allowing negative index
            return *(first + (step * i)); 
        }

        constexpr explicit operator bool() const { return step != 0 && first != last; }

        constexpr bool operator!=(const Iter&) const { return operator bool(); }

        constexpr void operator++() {
            if (step == 0) return;
            int inc = step > 0 ? 1 : -1;
            for (int i = 0; i != step; i += inc) {
                if (first == last) return;
                first += inc;
            }
        }

        constexpr auto& operator*() { return *first; }

        constexpr auto operator()() {
            using R = remove_reference_t<decltype(*first)>;
            auto valid = operator bool();
            auto res = valid ? *first : R{};
            if (valid) operator++();
            return res;
        }
    };

    template <typename Iterator> constexpr auto
    iter(Iterator first, Iterator last, int step = 1) { return Iter(first, last, step); }

    template <typename Container> constexpr auto
    iter(Container& cont, int step = 1) 
    { return step >= 0 ? Iter(etl::begin(cont), etl::end(cont), step) : Iter(etl::end(cont) - 1, etl::begin(cont) - 1, step); }

    /// python-like range
    template <typename T, typename U = remove_unsigned_t<T>>
    class Range {
        T first, last;
        U step;

    public:
        constexpr Range(T first, T last, U step) : first(first), last(last), step(step) {}

        [[nodiscard]] constexpr size_t len() const { return operator bool() ? (last - first) / step : 0; }
        constexpr Range begin() const { return *this; }
        constexpr Range end()   const { return *this; }

        constexpr explicit operator bool() const { return step == U(0) ? false : step < U(0) ? first > last : first < last; }
        constexpr bool operator!=(const Range&) const { return operator bool(); }
        constexpr void operator++() { first += step; }

        constexpr T operator*() { return first; }

        constexpr T operator()() { 
            auto valid = operator bool();
            auto res = valid ? first : T();
            if (valid) operator++();
            return res;
        }
    };

    template <typename T> constexpr enable_if_t<is_arithmetic_v<T>, Range<T>>
    range(T last) { return Range(T(0), last, remove_unsigned_t<T>(1)); }

    template <typename T> constexpr enable_if_t<is_arithmetic_v<T>, Range<T>>
    range(T first, T last, remove_unsigned_t<T> step = 1) { return Range(first, last, step); }

    /// python-like enumerate
    template <typename Iterator>
    class Enumerate {
        Iter<Iterator> iterator;
        int cnt;

    public:
        constexpr Enumerate(Iterator first, Iterator last, int cnt = 0, int step = 1)
        : iterator(first, last, step), cnt(cnt) {}

        constexpr Enumerate(Iter<Iterator> iterator, int cnt = 0)
        : iterator(iterator), cnt(cnt) {}  
 
        [[nodiscard]] constexpr size_t len() { return iterator.len(); }
        constexpr Enumerate begin() const { return *this; }
        constexpr Enumerate end()   const { return *this; }

        constexpr explicit operator bool() const { return bool(iterator); }
        constexpr bool operator!=(const Enumerate&) const { return operator bool(); }
        constexpr void operator++() { ++iterator; ++cnt; }

        constexpr auto operator*() { return etl::pair<int, decltype(*iterator)>(cnt, *iterator); }
        
        constexpr auto operator()() {
            using R = etl::Pair<int, remove_reference_t<decltype(*iterator)>>;
            auto valid = operator bool();
            auto res = valid ? R{cnt, *iterator} : R{};
            if (valid) operator++();
            return res;
        }
    };

    template <typename Iterator> constexpr auto
    enumerate(Iterator first, Iterator last, int cnt = 0, int step = 1) { return Enumerate(first, last, cnt, step); }

    template <typename Container> constexpr auto
    enumerate(Container& cont, int cnt = 0, int step = 1) { return Enumerate(etl::begin(cont), etl::end(cont), cnt, step); }

    /// python-like zip
    template <typename Iterator1, typename Iterator2>
    class Zip2 {
        Pair<Iter<Iterator1>, Iter<Iterator2>> iterators;

    public:
        constexpr Zip2(Iter<Iterator1> iter1, Iter<Iterator2> iter2)
        : iterators { iter1, iter2 } {}

        [[nodiscard]] constexpr size_t len() { 
            auto [x, y] = etl::pair(iterators.x.len(), iterators.y.len());
            return x < y ? x : y; 
        }
        constexpr Zip2 begin() const { return *this; }
        constexpr Zip2 end()   const { return *this; }

        constexpr explicit operator bool() const { return bool(iterators.x) && bool(iterators.y); }
        constexpr bool operator!=(const Zip2&) const { return operator bool(); }
        constexpr void operator++() { ++iterators.x; ++iterators.y; }

        constexpr Pair<decltype(*iterators.x), decltype(*iterators.y)> operator*() { 
            return {*iterators.x, *iterators.y};
        }

        constexpr auto operator()() {
            using R = Pair<remove_reference_t<decltype(*iterators.x)>, 
                           remove_reference_t<decltype(*iterators.y)>>;
            auto valid = operator bool();
            auto res = valid ? R{*iterators.x, *iterators.y} : R{}; 
            if (valid) operator++(); 
            return res; 
        }
    };

    template <typename Iterator1, typename Iterator2> constexpr auto
    zip(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2)
    { return Zip2(etl::iter(first1, last1), etl::iter(first2, last2)); }

    template <typename Container1, typename Container2> constexpr auto
    zip(Container1& cont1, Container2& cont2) { return Zip2(etl::iter(cont1), etl::iter(cont2)); }


    template <typename Iterator1, typename Iterator2, typename Iterator3>
    class Zip3 {
        Triple<Iter<Iterator1>, Iter<Iterator2>, Iter<Iterator3>> iterators;

    public:
        constexpr Zip3(Iter<Iterator1> iter1, Iter<Iterator2> iter2, Iter<Iterator3> iter3)
        : iterators { iter1, iter2, iter3 } {}

        [[nodiscard]] constexpr size_t len() {
            auto [x, y, z] = etl::pair(iterators.x.len(), iterators.y.len(), iterators.z.len());
            auto xy = x < y ? x : y;
            return xy < z ? xy : z;  
        }
        constexpr Zip3 begin() const { return *this; }
        constexpr Zip3 end()   const { return *this; }
        
        constexpr explicit operator bool() const { return bool(iterators.x) && bool(iterators.y) && bool(iterators.z); }
        constexpr bool operator!=(const Zip3&) const { return operator bool(); }
        constexpr void operator++() { ++iterators.x; ++iterators.y; ++iterators.z; }

        constexpr Triple<decltype(*iterators.x), decltype(*iterators.y), decltype(*iterators.z)> operator*() { 
            return {*iterators.x, *iterators.y, *iterators.z}; 
        }

        constexpr auto operator()() { 
            using R = Triple<remove_reference_t<decltype(*iterators.x)>, 
                             remove_reference_t<decltype(*iterators.y)>,
                             remove_reference_t<decltype(*iterators.z)>>;
            auto valid = operator bool();
            auto res = valid ? R{*iterators.x, *iterators.y, *iterators.z} : R{}; 
            if (valid) operator++(); 
            return res; 
        }
    };
 
    template <typename Iterator1, typename Iterator2, typename Iterator3> constexpr auto
    zip(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2, Iterator3 first3, Iterator3 last3)
    { return Zip3(etl::iter(first1, last1), etl::iter(first2, last2), etl::iter(first3, last3)); }

    template <typename Container1, typename Container2, typename Container3> constexpr auto
    zip(Container1& cont1, Container2& cont2, Container3& cont3) 
    { return Zip3(etl::iter(cont1), etl::iter(cont2), etl::iter(cont3)); }

    /// python-like reversed
    template <typename Container> constexpr auto
    reversed(const Container& cont) { return etl::iter(cont, -1); }

}

#endif //ETL_UTILITY_H
