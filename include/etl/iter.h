#ifndef ETL_ITER_H
#define ETL_ITER_H

#include "etl/type_traits.h"
#include "etl/utility_basic.h"

namespace Project::etl {

    /// python-like iter
    template <typename Iterator>
    class Iter {
        static_assert(etl::is_iterator_v<Iterator>, "The type has to be an iterator");
        Iterator first, last;
        int step;

    public:
        constexpr Iter(Iterator first, Iterator last, int step = 1)
            : first(first)
            , last(last)
            , step(step) {}

        constexpr const Iter& begin() const& { return *this; }
        constexpr const Iter& end()   const& { return *this; }
        constexpr const Iter& iter()  const& { return *this; }

        constexpr Iter begin() && { return etl::move(*this); }
        constexpr Iter end()   && { return etl::move(*this); }
        constexpr Iter iter()  && { return etl::move(*this); }

        constexpr size_t len() const {
            if constexpr (etl::has_operator_minus_v<Iterator>) {
                return step == 0 ? 0
                        : step > 0 ? (last - first) / step
                        : (first - last) / (-step);
            } else {
                size_t cnt = 0;
                for (Iterator it = first; it != last; it = increment_helper(step))
                    ++cnt;
                return cnt;
            }
        }

        constexpr decltype(auto) operator[](int i) {
            if (i < 0) { i = len() + i; } // allowing negative index
            if (i < 0) { etl::remove_reference_t<decltype(*first)>* res = nullptr; return *res; }
            return *increment_helper(step * i);
        }

        constexpr decltype(auto) operator[](int i) const {
            if (i < 0) { i = len() + i; } // allowing negative index
            if (i < 0) { etl::remove_reference_t<decltype(*first)>* res = nullptr; return *res; }
            return *increment_helper(step * i);
        }

        constexpr explicit operator bool() const {
            return step != 0 && first != last;
        }

        constexpr bool operator!=(const Iter&) const {
            return operator bool();
        }

        constexpr void operator++() {
            first = increment_helper(step);
        }

        constexpr decltype(auto) operator*() {
            return *first;
        }

        constexpr decltype(auto) operator()() {
            using R = decltype(operator*());
            if (!operator bool()) {
                if constexpr (etl::is_reference_v<R>) {
                    // undefined behaviour
                    return *static_cast<etl::add_pointer_t<etl::remove_reference_t<R>>>(nullptr);
                } else if constexpr (etl::has_empty_constructor_v<R>) {
                    // avoid segfault
                    return R();
                } else {
                    // segfault
                    return R(*static_cast<R*>(nullptr));
                }
            }

            decltype(auto) res = operator*();
            operator++();
            return res;
        }

    private:
        constexpr Iterator increment_helper(int inc) const {
            Iterator res = first;
            if (inc > 0) {
                for (int i = 0; i != inc && res != last; i += 1) {
                    ++res;
                }
            } else if (inc < 0) {
                for (int i = 0; i != -inc && res != last; i += 1) {
                    if constexpr (etl::has_prefix_decrement_v<Iterator>)
                        --res;
                    else
                        break; // not supposed to be here
                }
            }
            return res;
        }
    };

    /// create iter object from iterator
    template <typename Iterator, typename = enable_if_t<etl::has_prefix_decrement_v<Iterator>>> constexpr auto
    iter(Iterator first, Iterator last, int step = 1) { return Iter(first, last, step); }

    template <typename T, size_t N> constexpr auto
    iter(T(&arr)[N]) { return Iter(arr, arr + N, 1); }

    template <typename T> struct trait_iterable<T, etl::enable_if_t<
        !etl::detail::trait_has_iter<T>::value && etl::detail::trait_has_begin_end<T>::value
    >> : etl::true_type {
        template <typename U> static constexpr decltype(auto) iter(U& t) { return etl::Iter(etl::begin(t), etl::end(t), 1); }
    };

    template <typename T> struct trait_reverse_iterable<T, etl::enable_if_t<
        !etl::detail::trait_has_reversed<T>::value && !etl::detail::trait_has_rbegin_rend<T>::value && etl::detail::trait_has_begin_end<T>::value
    >> : etl::true_type {
        template <typename U> static constexpr decltype(auto) reversed(U& t) { return etl::Iter(t.end() - 1, t.begin() - 1, -1); }
    };

    template <typename T> struct trait_reverse_iterable<T, etl::enable_if_t<
        !etl::detail::trait_has_reversed<T>::value && etl::detail::trait_has_rbegin_rend<T>::value
    >> : etl::true_type {
        template <typename U> static constexpr decltype(auto) reversed(U& t) { return etl::Iter(t.rbegin(), t.rend(), 1); }
    };

    template <typename T> struct remove_extent<Iter<T>> { typedef remove_reference_t<decltype(*etl::declval<T>())> type; };
    template <typename T> struct remove_extent<const Iter<T>> { typedef remove_extent_t<Iter<T>> type; };
    template <typename T> struct remove_extent<volatile Iter<T>> { typedef remove_extent_t<Iter<T>> type; };
    template <typename T> struct remove_extent<const volatile Iter<T>> { typedef remove_extent_t<Iter<T>> type; };

}

#endif // ETL_ITER_H
