#ifndef ETL_ALGORITHM_H
#define ETL_ALGORITHM_H

#include "etl/utility.h"

namespace Project::etl {
    /// finds the first element satisfying specific criteria
    template <class Iterator, class T> constexpr auto
    find(Iterator first, Iterator last, const T& value) {
        for (; first != last; ++first) if (*first == value) return first;
        return last;
    }
    template <class Container, class T> constexpr auto
    find(const Container& cont, const T& value) {
        return etl::find(etl::begin(cont), etl::end(cont), value);
    }
    template <class Iterator, class UnaryPredicate> constexpr auto
    find_if(Iterator first, Iterator last, UnaryPredicate fn) {
        for (; first != last; ++first) if (fn(*first)) return first;
        return last;
    }
    template <class Container, class UnaryPredicate> constexpr auto
    find_if(Container& cont, UnaryPredicate fn) {
        return etl::find_if(etl::begin(cont), etl::end(cont), fn);
    }
    template <class Iterator, class UnaryPredicate> constexpr auto
    find_if_not(Iterator first, Iterator last, UnaryPredicate fn) {
        for (; first != last; ++first) if (!fn(*first)) return first;
        return last;
    }
    template <class Container, class UnaryPredicate> constexpr auto
    find_if_not(Container& cont, UnaryPredicate fn) {
        return etl::find_if_not(etl::begin(cont), etl::end(cont), fn);
    }

    /// checks if a predicate is true for all, any or none of the elements in a range
    template <class Iterator, class T> constexpr bool
    all(Iterator first, Iterator last, const T& value) {
        for (; first != last; ++first) if (*first != value) return false;
        return true;
    }
    template <class Container, class T> constexpr bool
    all(const Container& cont, const T& value) {
        return etl::all(etl::begin(cont), etl::end(cont), value);
    }
    template <class Iterator, class T> constexpr bool
    any(Iterator first, Iterator last, const T& value) {
        for (; first != last; ++first) if (*first == value) return true;
        return false;
    }
    template <class Container, class T> constexpr bool
    any(const Container& cont, const T& value) {
        return etl::any(etl::begin(cont), etl::end(cont), value);
    }
    template <class Iterator, class T> constexpr bool
    none(Iterator first, Iterator last, const T& value) {
        for (; first != last; ++first) if (*first == value) return false;
        return true;
    }
    template <class Container, class T> constexpr bool
    none(const Container& cont, const T& value) {
        return etl::none(etl::begin(cont), etl::end(cont), value);
    }
    template <class Iterator, class UnaryPredicate> constexpr bool
    all_if(Iterator first, Iterator last, UnaryPredicate fn) {
        return etl::find_if_not(first, last, fn) == last;
    }
    template <class Container, class UnaryPredicate> constexpr bool
    all_if(Container& cont, UnaryPredicate fn) {
        return etl::all_if(etl::begin(cont), etl::end(cont), fn);
    }
    template <class Iterator, class UnaryPredicate> constexpr bool
    any_if(Iterator first, Iterator last, UnaryPredicate fn) {
        return etl::find_if(first, last, fn) != last;
    }
    template <class Container, class UnaryPredicate> constexpr bool
    any_if(Container& cont, UnaryPredicate fn) {
        return etl::any_if(etl::begin(cont), etl::end(cont), fn);
    }
    template <class Iterator, class UnaryPredicate> constexpr bool
    none_if(Iterator first, Iterator last, UnaryPredicate fn) {
        return etl::find_if(first, last, fn) == last;
    }
    template <class Container, class UnaryPredicate> constexpr bool
    none_if(Container& cont, UnaryPredicate fn) {
        return etl::none_if(etl::begin(cont), etl::end(cont), fn);
    }

    /// compare all, any or none of between a container and a generator
    template <class Iterator, class Generator> constexpr bool
    all_of(Iterator first, Iterator last, Generator fn) {
        for (; first != last; ++first) if (*first != fn()) return false;
        return true;
    }
    template <class Container, class Generator> constexpr bool
    all_of(const Container& cont, Generator fn) {
        return etl::all_of(etl::begin(cont), etl::end(cont), fn);
    }
    template <class Iterator, class Generator> constexpr bool
    any_of(Iterator first, Iterator last, Generator fn) {
        for (; first != last; ++first) if (*first == fn()) return true;
        return false;
    }
    template <class Container, class Generator> constexpr bool
    any_of(const Container& cont, Generator fn) {
        return etl::any_of(etl::begin(cont), etl::end(cont), fn);
    }
    template <class Iterator, class Generator> constexpr bool
    none_of(Iterator first, Iterator last, Generator fn) {
        for (; first != last; ++first) if (*first == fn()) return false;
        return true;
    }
    template <class Container, class Generator> constexpr bool
    none_of(const Container& cont, Generator fn) {
        return etl::none_of(etl::begin(cont), etl::end(cont), fn);
    }

    /// compare all, any or none of between two containers
    template <class Container1, class Container2> constexpr bool
    compare_all(const Container1& cont1, const Container2& cont2) {
        if (etl::len(cont1) != etl::len(cont2)) return false;
        for (auto [x, y] : etl::zip(cont1, cont2)) if (x != y) return false;
        return true;
    }
    template <class Container1, class Container2> constexpr bool
    compare_any(const Container1& cont1, const Container2& cont2) {
        for (auto [x, y] : etl::zip(cont1, cont2)) if (x == y) return true;
        return false;
    }
    template <class Container1, class Container2> constexpr bool
    compare_none(const Container1& cont1, const Container2& cont2) {
        for (auto [x, y] : etl::zip(cont1, cont2)) if (x == y) return false;
        return true;
    }

    /// applies function fn(item) to a range of elements
    template <class Iterator, class UnaryFunction> constexpr auto
    foreach(Iterator first, Iterator last, UnaryFunction fn) {
        for (; first != last; ++first) fn(*first);
        return fn;
    }
    template <class Container, class UnaryFunction> constexpr auto
    foreach(Container& cont, UnaryFunction fn) {
        return etl::foreach(etl::begin(cont), etl::end(cont), fn);
    }

    /// applies function fn(item, result) to a range of elements
    template <class Iterator, class UnaryFunction, class R> constexpr auto
    fold(Iterator first, Iterator last, UnaryFunction fn, R& result) {
        for (; first != last; ++first) fn(*first, result);
        return fn;
    }
    template <class Container, class UnaryFunction, class R> constexpr auto
    fold(Container& cont, UnaryFunction fn, R& result) {
        return etl::fold(etl::begin(cont), etl::end(cont), fn, result);
    }
    
    /// copy-assigns the given value to every element in a range
    template <class Iterator, class T> constexpr void
    fill(Iterator first, Iterator last, const T& value) {
        for (; first != last; ++first) *first = value;
    }
    template <class Container, class T> constexpr void
    fill(Container& cont, const T& value) {
        for (auto& x : cont) x = value;
    }
    
    /// assigns the results of successive function calls to every element in a range
    template <class Iterator, class Generator> constexpr void
    generate(Iterator first, Iterator last, Generator fn) {
        for (; first != last; ++first) *first = fn();
    }
    template <class Container, class Generator> constexpr void
    generate(Container& cont, Generator fn) {
        for (auto& x : cont) x = fn();
    }

    /// returns the number of elements satisfying specific criteria
    template <class Iterator, class T> constexpr int
    count(Iterator first, Iterator last, const T& value) {
        int res = 0;
        for (; first != last; ++first) if (*first == value) ++res;
        return res;
    }
    template <class Container, class T> constexpr int
    count(const Container& cont, const T& value) {
        return etl::count(etl::begin(cont), etl::end(cont), value);
    }
    template <class Iterator, class UnaryPredicate> constexpr int
    count_if(Iterator first, Iterator last, UnaryPredicate fn) {
        int res = 0;
        for (; first != last; ++first) if (fn(*first)) ++res;
        return res;
    }
    template <class Container, class UnaryPredicate> constexpr int
    count_if(Container& cont, UnaryPredicate fn) {
        return etl::count_if(etl::begin(cont), etl::end(cont), fn);
    }

    /// copies a range of elements to a new location
    template <class Iterator, class IteratorDest> constexpr auto
    copy(Iterator first, Iterator last, IteratorDest dest) {
        for (; first != last && dest != nullptr; ++first, ++dest) *dest = *first;
        return dest;
    }
    template <class Container, class ContainerDest> constexpr void
    copy(const Container& cont, ContainerDest& dest) {
        for (auto [x, y] : etl::zip(cont, dest)) y = x;
    }

    template <class Iterator, class IteratorDest, class UnaryPredicate> constexpr auto
    copy_if(Iterator first, Iterator last, IteratorDest dest, UnaryPredicate fn) {
        for (; first != last && dest != nullptr; ++first) if (fn(*first)) { *dest = *first; ++dest; }
        return dest;
    }
    template <class Container, class ContainerDest, class UnaryPredicate> constexpr auto
    copy_if(const Container& cont, ContainerDest& dest, UnaryPredicate fn) {
        return etl::copy_if(etl::begin(cont), etl::end(cont), etl::begin(dest), fn);
    }

    /// moves a range of elements to a new location
    template <class Iterator, class IteratorDest> constexpr auto
    move(Iterator first, Iterator last, IteratorDest dest) {
        for (; first != last && dest != nullptr; ++first, ++dest) *dest = etl::move(*first);
        return dest;
    }
    template <class Container, class ContainerDest> constexpr void
    move(const Container& cont, ContainerDest& dest) {
        for (auto [x, y] : etl::zip(cont, dest)) y = etl::move(x);
    }

    /// swap
    template <class T> constexpr void
    swap(T& a, T& b) {
        T temp(etl::move(a));
        a = etl::move(b);
        b = etl::move(temp);
    }
    template <class Iterator1, class Iterator2> constexpr void
    swap_element(Iterator1 first, Iterator1 last, Iterator2 dest) {
        for (; first != last && dest != nullptr; ++first, ++dest) etl::swap(*first, *dest);
    }
    template <class Container1, class Container2> constexpr void
    swap_element(Container1& cont1, Container2& cont2) {
        for (auto [x, y] : etl::zip(cont1, cont2)) etl::swap(x, y);
    }

    /// replaces all values satisfying specific criteria with another value @{
    template <class Iterator, class T> constexpr void
    replace(Iterator first, Iterator last, const T& old, const T& value) {
        for (; first != last; ++first) if (*first == old) *first = value;
    }
    template <class Container, class T> constexpr void
    replace(Container& cont, const T& old, const T& value) {
        etl::replace(etl::begin(cont), etl::end(cont), old, value);
    }
    template <class Iterator, class UnaryPredicate, class T> void
    replace_if(Iterator first, Iterator last, UnaryPredicate fn, const T& value) {
        for (; first != last; ++first) if (fn(*first)) *first = value;
    }
    template <class Container, class UnaryPredicate, class T> constexpr void
    replace_if(Container& cont, UnaryPredicate fn, const T& value) {
        etl::replace_if(etl::begin(cont), etl::end(cont), fn, value);
    }

    /// returns the greater of the given values
    template <class T1, class T2, class... Tn> constexpr auto
    max(const T1& val1, const T2& val2, const Tn&... vals) {
        if constexpr (sizeof...(vals) == 0) return val1 > val2 ? val1 : val2;
        else return etl::max(etl::max(val1, val2), vals...);
    }

    /// returns the smaller of the given values
    template <class T1, class T2, class... Tn> constexpr auto
    min(const T1& val1, const T2& val2, const Tn&... vals) {
        if constexpr (sizeof...(vals) == 0) return val1 < val2 ? val1 : val2;
        else return etl::min(etl::min(val1, val2), vals...);
    }

    /// returns the largest element in a range
    template <class Iterator> constexpr auto
    max_element(Iterator first, Iterator last) {
        if (first == last) return *last;
        auto largest = first++;
        for (; first != last; ++first) if (*first > *largest) largest = first;
        return *largest;
    }
    template <class Container> constexpr auto
    max_element(const Container& cont) {
        return etl::max_element(etl::begin(cont), etl::end(cont));
    }

    /// returns the smallest element in a range
    template <class Iterator> constexpr auto
    min_element(Iterator first, Iterator last) {
        if (first == last) return *last;
        auto smallest = first++;
        for (; first != last; ++first) if (*first < *smallest) smallest = first;
        return *smallest;
    }
    template <class Container> constexpr auto
    min_element(const Container& cont) {
        return etl::min_element(etl::begin(cont), etl::end(cont));
    }

    /// clamps a value between a pair of boundary values
    template <class T> constexpr T
    clamp(const T& x, const T& lo, const T& hi) {
        auto low = etl::min(lo, hi);
        auto high = etl::max(lo, hi);
        return x > high ? high : x < low ? low : x;
    }
}

#endif //ETL_ALGORITHM_H
