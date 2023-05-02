#ifndef ETL_ALGORITHM_H
#define ETL_ALGORITHM_H

#include "etl/utility.h"

namespace Project::etl {
    /// finds the first element satisfying specific criteria
    template <typename Iterator, typename T> constexpr auto
    find(Iterator first, Iterator last, const T& value) {
        for (; first != last; ++first) if (*first == value) return first;
        return last;
    }
    template <typename Container, typename T> constexpr auto
    find(Container&& cont, const T& value) {
        return etl::find(etl::begin(cont), etl::end(cont), value);
    }
    template <typename Iterator, typename UnaryPredicate> constexpr auto
    find_if(Iterator first, Iterator last, UnaryPredicate fn) {
        for (; first != last; ++first) if (fn(*first)) return first;
        return last;
    }
    template <typename Container, typename UnaryPredicate> constexpr auto
    find_if(Container&& cont, UnaryPredicate fn) {
        return etl::find_if(etl::begin(cont), etl::end(cont), fn);
    }
    template <typename Iterator, typename UnaryPredicate> constexpr auto
    find_if_not(Iterator first, Iterator last, UnaryPredicate fn) {
        for (; first != last; ++first) if (!fn(*first)) return first;
        return last;
    }
    template <typename Container, typename UnaryPredicate> constexpr auto
    find_if_not(Container&& cont, UnaryPredicate fn) {
        return etl::find_if_not(etl::begin(cont), etl::end(cont), fn);
    }

    /// checks if a predicate is true for all, any or none of the elements in a range
    template <typename Iterator, typename T> constexpr bool
    all(Iterator first, Iterator last, const T& value) {
        for (; first != last; ++first) if (*first != value) return false;
        return true;
    }
    template <typename Container, typename T> constexpr bool
    all(Container&& cont, const T& value) {
        return etl::all(etl::begin(cont), etl::end(cont), value);
    }
    template <typename Iterator, typename T> constexpr bool
    any(Iterator first, Iterator last, const T& value) {
        for (; first != last; ++first) if (*first == value) return true;
        return false;
    }
    template <typename Container, typename T> constexpr bool
    any(Container&& cont, const T& value) {
        return etl::any(etl::begin(cont), etl::end(cont), value);
    }
    template <typename Iterator, typename T> constexpr bool
    none(Iterator first, Iterator last, const T& value) {
        for (; first != last; ++first) if (*first == value) return false;
        return true;
    }
    template <typename Container, typename T> constexpr bool
    none(Container&& cont, const T& value) {
        return etl::none(etl::begin(cont), etl::end(cont), value);
    }
    template <typename Iterator, typename UnaryPredicate> constexpr bool
    all_if(Iterator first, Iterator last, UnaryPredicate fn) {
        return etl::find_if_not(first, last, fn) == last;
    }
    template <typename Container, typename UnaryPredicate> constexpr bool
    all_if(Container&& cont, UnaryPredicate fn) {
        return etl::all_if(etl::begin(cont), etl::end(cont), fn);
    }
    template <typename Iterator, typename UnaryPredicate> constexpr bool
    any_if(Iterator first, Iterator last, UnaryPredicate fn) {
        return etl::find_if(first, last, fn) != last;
    }
    template <typename Container, typename UnaryPredicate> constexpr bool
    any_if(Container&& cont, UnaryPredicate fn) {
        return etl::any_if(etl::begin(cont), etl::end(cont), fn);
    }
    template <typename Iterator, typename UnaryPredicate> constexpr bool
    none_if(Iterator first, Iterator last, UnaryPredicate fn) {
        return etl::find_if(first, last, fn) == last;
    }
    template <typename Container, typename UnaryPredicate> constexpr bool
    none_if(Container&& cont, UnaryPredicate fn) {
        return etl::none_if(etl::begin(cont), etl::end(cont), fn);
    }

    /// compare all, any or none of between a container and a generator
    template <typename Iterator, typename Generator> constexpr bool
    all_of(Iterator first, Iterator last, Generator fn) {
        for (; first != last; ++first) if (*first != fn()) return false;
        return true;
    }
    template <typename Container, typename Generator> constexpr bool
    all_of(Container&& cont, Generator fn) {
        return etl::all_of(etl::begin(cont), etl::end(cont), fn);
    }
    template <typename Iterator, typename Generator> constexpr bool
    any_of(Iterator first, Iterator last, Generator fn) {
        for (; first != last; ++first) if (*first == fn()) return true;
        return false;
    }
    template <typename Container, typename Generator> constexpr bool
    any_of(Container&& cont, Generator fn) {
        return etl::any_of(etl::begin(cont), etl::end(cont), fn);
    }
    template <typename Iterator, typename Generator> constexpr bool
    none_of(Iterator first, Iterator last, Generator fn) {
        for (; first != last; ++first) if (*first == fn()) return false;
        return true;
    }
    template <typename Container, typename Generator> constexpr bool
    none_of(Container&& cont, Generator fn) {
        return etl::none_of(etl::begin(cont), etl::end(cont), fn);
    }

    /// compare all, any or none of between two containers
    template <typename Container1, typename Container2> constexpr bool
    compare_all(Container1&& cont1, Container2&& cont2) {
        if constexpr (has_len_v<remove_reference_t<Container1>> && has_len_v<remove_reference_t<Container2>>)
            if (etl::len(cont1) != etl::len(cont2)) 
                return false;

        for (auto [x, y] : etl::zip(cont1, cont2)) if (x != y) return false;
        return true;
    }
    template <typename Container1, typename Container2> constexpr bool
    compare_any(Container1&& cont1, Container2&& cont2) {
        for (auto [x, y] : etl::zip(cont1, cont2)) if (x == y) return true;
        return false;
    }
    template <typename Container1, typename Container2> constexpr bool
    compare_none(Container1&& cont1, Container2&& cont2) {
        for (auto [x, y] : etl::zip(cont1, cont2)) if (x == y) return false;
        return true;
    }

    /// applies function fn(item) to a range of elements
    template <typename Iterator, typename UnaryFunction> constexpr auto
    foreach(Iterator first, Iterator last, UnaryFunction fn) {
        for (; first != last; ++first) fn(*first);
        return fn;
    }
    template <typename Container, typename UnaryFunction> constexpr auto
    foreach(Container&& cont, UnaryFunction fn) {
        return etl::foreach(etl::begin(cont), etl::end(cont), fn);
    }

    /// applies function fn(item, result) to a range of elements
    template <typename Iterator, typename UnaryFunction, typename R> constexpr auto
    fold(Iterator first, Iterator last, UnaryFunction fn, R& result) {
        for (; first != last; ++first) fn(*first, result);
        return fn;
    }
    template <typename Container, typename UnaryFunction, typename R> constexpr auto
    fold(Container&& cont, UnaryFunction fn, R& result) {
        return etl::fold(etl::begin(cont), etl::end(cont), fn, result);
    }
    
    /// copy-assigns the given value to every element in a range
    template <typename Iterator, typename T> constexpr void
    fill(Iterator first, Iterator last, const T& value) {
        for (; first != last; ++first) *first = value;
    }
    template <typename Container, typename T> constexpr void
    fill(Container&& cont, const T& value) {
        for (auto& x : cont) x = value;
    }
    
    /// assigns the results of successive function calls to every element in a range
    template <typename Iterator, typename Generator> constexpr void
    generate(Iterator first, Iterator last, Generator fn) {
        for (; first != last; ++first) *first = fn();
    }
    template <typename Container, typename Generator> constexpr void
    generate(Container&& cont, Generator fn) {
        for (auto& x : cont) x = fn();
    }

    /// returns the number of elements satisfying specific criteria
    template <typename Iterator, typename T> constexpr int
    count(Iterator first, Iterator last, const T& value) {
        int res = 0;
        for (; first != last; ++first) if (*first == value) ++res;
        return res;
    }
    template <typename Container, typename T> constexpr int
    count(Container&& cont, const T& value) {
        return etl::count(etl::begin(cont), etl::end(cont), value);
    }
    template <typename Iterator, typename UnaryPredicate> constexpr int
    count_if(Iterator first, Iterator last, UnaryPredicate fn) {
        int res = 0;
        for (; first != last; ++first) if (fn(*first)) ++res;
        return res;
    }
    template <typename Container, typename UnaryPredicate> constexpr int
    count_if(Container&& cont, UnaryPredicate fn) {
        return etl::count_if(etl::begin(cont), etl::end(cont), fn);
    }

    /// copies a range of elements to a new location
    template <typename Iterator, typename IteratorDest> constexpr auto
    copy(Iterator first, Iterator last, IteratorDest dest) {
        for (; first != last && dest != nullptr; ++first, ++dest) *dest = *first;
        return dest;
    }
    template <typename Container, typename ContainerDest> constexpr auto
    copy(Container&& cont, ContainerDest& dest) {
        return copy(etl::begin(cont), etl::end(cont), etl::begin(dest));
    }

    template <typename Iterator, typename IteratorDest, typename UnaryPredicate> constexpr auto
    copy_if(Iterator first, Iterator last, IteratorDest dest, UnaryPredicate fn) {
        for (; first != last && dest != nullptr; ++first) if (fn(*first)) { *dest = *first; ++dest; }
        return dest;
    }
    template <typename Container, typename ContainerDest, typename UnaryPredicate> constexpr auto
    copy_if(Container&& cont, ContainerDest& dest, UnaryPredicate fn) {
        return etl::copy_if(etl::begin(cont), etl::end(cont), etl::begin(dest), fn);
    }

    /// moves a range of elements to a new location
    template <typename Iterator, typename IteratorDest> constexpr auto
    move(Iterator first, Iterator last, IteratorDest dest) {
        for (; first != last && dest != nullptr; ++first, ++dest) *dest = etl::move(*first);
        return dest;
    }
    template <typename Container, typename ContainerDest> constexpr void
    move(Container&& cont, ContainerDest& dest) {
        for (auto [x, y] : etl::zip(cont, dest)) y = etl::move(x);
    }

    /// swap
    template <typename Iterator1, typename Iterator2> constexpr void
    swap_element(Iterator1 first, Iterator1 last, Iterator2 dest) {
        for (; first != last && dest != nullptr; ++first, ++dest) etl::swap(*first, *dest);
    }
    template <typename Container1, typename Container2> constexpr void
    swap_element(Container1&& cont1, Container2&& cont2) {
        for (auto [x, y] : etl::zip(cont1, cont2)) etl::swap(x, y);
    }

    /// replaces all values satisfying specific criteria with another value @{
    template <typename Iterator, typename T> constexpr void
    replace(Iterator first, Iterator last, const T& old, const T& value) {
        for (; first != last; ++first) if (*first == old) *first = value;
    }
    template <typename Container, typename T> constexpr void
    replace(Container&& cont, const T& old, const T& value) {
        etl::replace(etl::begin(cont), etl::end(cont), old, value);
    }
    template <typename Iterator, typename UnaryPredicate, typename T> void
    replace_if(Iterator first, Iterator last, UnaryPredicate fn, const T& value) {
        for (; first != last; ++first) if (fn(*first)) *first = value;
    }
    template <typename Container, typename UnaryPredicate, typename T> constexpr void
    replace_if(Container&& cont, UnaryPredicate fn, const T& value) {
        etl::replace_if(etl::begin(cont), etl::end(cont), fn, value);
    }

    /// returns the greater of the given values
    template <typename T1, typename T2, typename... Tn> constexpr auto
    max(const T1& val1, const T2& val2, const Tn&... vals) {
        if constexpr (sizeof...(vals) == 0) return val1 > val2 ? val1 : val2;
        else return etl::max(etl::max(val1, val2), vals...);
    }

    /// returns the smaller of the given values
    template <typename T1, typename T2, typename... Tn> constexpr auto
    min(const T1& val1, const T2& val2, const Tn&... vals) {
        if constexpr (sizeof...(vals) == 0) return val1 < val2 ? val1 : val2;
        else return etl::min(etl::min(val1, val2), vals...);
    }

    /// returns the sum of all arguments
    template <typename T, typename... Ts> constexpr auto
    sum(const T& val, const Ts&... vals) {
        if constexpr (sizeof...(vals) == 0) return val;
        else return val + etl::sum(vals...);
    }

    /// returns the largest element in a range
    template <typename Iterator> constexpr auto
    max_element(Iterator first, Iterator last) {
        if (first == last) return *last;
        auto largest = first++;
        for (; first != last; ++first) if (*first > *largest) largest = first;
        return *largest;
    }
    template <typename Container> constexpr auto
    max_element(Container&& cont) {
        return etl::max_element(etl::begin(cont), etl::end(cont));
    }

    /// returns the smallest element in a range
    template <typename Iterator> constexpr auto
    min_element(Iterator first, Iterator last) {
        if (first == last) return *last;
        auto smallest = first++;
        for (; first != last; ++first) if (*first < *smallest) smallest = first;
        return *smallest;
    }
    template <typename Container> constexpr auto
    min_element(Container&& cont) {
        return etl::min_element(etl::begin(cont), etl::end(cont));
    }

    /// returns the sum of all elements in a range
    template <typename Iterator> constexpr auto
    sum_element(Iterator first, Iterator last) {
        using T = remove_reference_t<decltype(*first)>;
        using R = conditional_t<is_arithmetic_v<T>, conditional_t<is_floating_point_v<T>, double, long>, T>;
        using S = conditional_t<is_unsigned_v<R>, add_unsigned_t<R>, R>;
        
        if (first == last) return S{};
        S res = *first;
        for (++first; first != last; ++first) res += *first;
        return res;
    }
    template <typename Container> constexpr auto
    sum_element(Container&& cont) {
        return etl::sum_element(etl::begin(cont), etl::end(cont));
    }

    /// clamps a value between a pair of boundary values
    template <typename T, typename U, typename V> constexpr auto
    clamp(const T& x, const U& lo, const V& hi) {
        auto low = etl::min(lo, hi);
        auto high = etl::max(lo, hi);
        return x > high ? high : x < low ? low : x;
    }
}

#endif //ETL_ALGORITHM_H
