#ifndef ETL_ALGORITHM_H
#define ETL_ALGORITHM_H

#include "etl/utility.h"

namespace Project::etl {
    /// finds the first element satisfying specific criteria
    /// @{
    template <class Iterator, class T>
    constexpr auto find(Iterator first, Iterator last, const T& value) {
        for (; first != last; ++first) if (*first == value) return first;
        return last;
    }
    template <class Iterator, class UnaryPredicate>
    constexpr auto find_if(Iterator first, Iterator last, UnaryPredicate fn) {
        for (; first != last; ++first) if (fn(*first)) return first;
        return last;
    }
    template <class Iterator, class UnaryPredicate>
    constexpr auto find_if_not(Iterator first, Iterator last, UnaryPredicate fn) {
        for (; first != last; ++first) if (!fn(*first)) return first;
        return last;
    }
    /// @}

    /// checks if a predicate is true for all, any or none of the elements in a range
    template <class Iterator, class UnaryPredicate>
    constexpr bool all(Iterator first, Iterator last, UnaryPredicate fn) {
        return find_if_not(first, last, fn) == last;
    }
    template <class Iterator, class UnaryPredicate>
    constexpr bool any(Iterator first, Iterator last, UnaryPredicate fn) {
        return find_if(first, last, fn) != last;
    }
    template <class Iterator, class UnaryPredicate>
    constexpr bool none_of(Iterator first, Iterator last, UnaryPredicate fn) {
        return find_if(first, last, fn) == last;
    }
    /// @}

    /// applies a function fn(item) to a range of elements
    template <class Iterator, class UnaryFunction>
    constexpr auto foreach(Iterator first, Iterator last, UnaryFunction fn) {
        for (; first != last; ++first) fn(*first);
        return fn;
    }

    /// applies a function fn(item, result) to a range of elements
    template <class Iterator, class UnaryFunction, class R>
    constexpr auto fold(Iterator first, Iterator last, UnaryFunction fn, R& result) {
        for (; first != last; ++first) fn(*first, result);
        return fn;
    }

    /// returns the number of elements satisfying specific criteria
    template <class Iterator, class T>
    constexpr int count(Iterator first, Iterator last, const T& value) {
        int res = 0;
        for (; first != last; ++first) if (*first == value) ++res;
        return res;
    }
    template <class Iterator, class UnaryPredicate>
    constexpr int count_if(Iterator first, Iterator last, UnaryPredicate fn) {
        int res = 0;
        for (; first != last; ++first) if (fn(*first)) ++res;
        return res;
    }
    /// @}

    /// copies a range of elements to a new location
    template <class Iterator, class IteratorDest>
    constexpr auto copy(Iterator first, Iterator last, IteratorDest dest) {
        for (; first != last; ++first, ++dest) *dest = *first;
        return dest;
    }
    template <class Iterator, class IteratorDest, class UnaryPredicate>
    constexpr auto copy_if(Iterator first, Iterator last, IteratorDest dest, UnaryPredicate fn) {
        for (; first != last; ++first) if (fn(*first)) { *dest = *first; ++dest; }
        return dest;
    }
    /// @}

    /// remove reference
    /// @{
    template <typename T> struct remove_reference       { typedef T Type; };
    template <typename T> struct remove_reference<T&>   { typedef T Type; };
    template <typename T> struct remove_reference<T&&>  { typedef T Type; };
    /// @}

    /// moves to rvalue
    template<typename T>
    constexpr typename remove_reference<T>::Type&& move(T&& t) {
        return static_cast<typename remove_reference<T>::Type&&>(t);
    }

    /// moves a range of elements to a new location
    template <class Iterator, class IteratorDest>
    constexpr auto move(Iterator first, Iterator last, IteratorDest dest) {
        for (; first != last; ++first, ++dest) *dest = move(*first);
        return dest;
    }

    /// copy-assigns the given value to every element in a range
    template <class Iterator, class T>
    void fill(Iterator first, Iterator last, const T& value) {
        for (; first != last; ++first) *first = value;
    }

    /// assigns the results of successive function calls to every element in a range
    template <class Iterator, class Generator>
    void generate(Iterator first, Iterator last, Generator fn) {
        for (; first != last; ++first) *first = fn();
    }

    /// replaces all values satisfying specific criteria with another value
    /// @{
    template <class Iterator, class T>
    void replace(Iterator first, Iterator last, const T& old, const T& value) {
        for (; first != last; ++first) if (*first == old) *first = value;
    }
    template <class Iterator, class UnaryPredicate, class T>
    void replace_if(Iterator first, Iterator last, UnaryPredicate fn, const T& value) {
        for (; first != last; ++first) if (p(*first)) *first = value;
    }
    /// @}

    /// returns the greater of the given values
    template <class T1, class T2, class... Tn>
    constexpr auto max(const T1& val1, const T2& val2, const Tn&... vals) {
        if constexpr (sizeof...(vals) == 0) return val1 > val2 ? val1 : val2;
        else return max(max(val1, val2), vals...);
    }

    /// returns the smaller of the given values
    template <class T1, class T2, class... Tn>
    constexpr auto min(const T1& val1, const T2& val2, const Tn&... vals) {
        if constexpr (sizeof...(vals) == 0) return val1 < val2 ? val1 : val2;
        else return min(min(val1, val2), vals...);
    }

    /// returns the largest element in a range
    template <class Iterator>
    constexpr auto max_element(Iterator first, Iterator last) {
        if (first == last) return last;
        auto largest = first++;
        for (; first != last; ++first) if (*first > *largest) largest = first;
        return largest;
    }

    /// returns the largest element in a range
    template <class Iterator>
    constexpr auto min_element(Iterator first, Iterator last) {
        if (first == last) return last;
        auto smallest = first++;
        for (; first != last; ++first) if (*first < *smallest) smallest = first;
        return smallest;
    }

    /// clamps a value between a pair of boundary values
    template <class T>
    constexpr T clamp(const T& x, const T& lo, const T& hi) {
        auto low = min(lo, hi);
        auto high = max(lo, hi);
        return x > high ? high : x < low ? low : x;
    }

    /// computes absolute value
    template <class T> constexpr T absolute(const T& x) { return x < 0 ? -x : x; }

    /// interpolate x given [x1, y1] and [x2, y2]
    /// @param trim truncate the result to range (y1, y2). default = true
    /// @{
    template <class X, class Y>
    constexpr Y interpolate(const X& x, const X& x1, const X& x2, const Y& y1, const Y& y2, bool trim = true) {
        Y res = y1 + (Y) (static_cast<float>(y2 - y1) * static_cast<float>(x - x1) / static_cast<float>(x2 - x1));
        return trim ? clamp(res, y1, y2) : res;
    }
    template <class X, class Y>
    constexpr Y interpolate(const X& x, const Pair<X,Y>& p1, const Pair<X,Y>& p2, bool trim = true) {
        auto& [x1, y1] = p1;
        auto& [x2, y2] = p2;
        return interpolate(x, x1, x2, y1, y2, trim);
    }
    /// @}
}

#endif //ETL_ALGORITHM_H
