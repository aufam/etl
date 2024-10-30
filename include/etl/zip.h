#ifndef ETL_ZIP_H
#define ETL_ZIP_H

#include "etl/tuple.h"
#include "etl/iter.h"

namespace Project::etl {

    /// python-like zip
    template <typename... Sequences>
    class Zip {
        Tuple<Sequences...> sequences;

    public:
        constexpr explicit Zip(Sequences... seq) : sequences{seq...} {}

        constexpr const Zip& begin() const& { return *this; }
        constexpr const Zip& end()   const& { return *this; }
        constexpr const Zip& iter()  const& { return *this; }

        constexpr Zip begin() && { return etl::move(*this); }
        constexpr Zip end()   && { return etl::move(*this); }
        constexpr Zip iter()  && { return etl::move(*this); }

        constexpr explicit operator bool() const { return bool_helper_<0>(); }

        constexpr bool operator!=(const Zip&) const { return operator bool(); }

        constexpr void operator++() { inc_helper_<0>(); }

        constexpr auto operator*() { return deref_helper_(etl::index_sequence_for<Sequences...>{}); }
        constexpr auto operator*() const { return deref_helper_(etl::index_sequence_for<Sequences...>{}); }

        constexpr auto operator()() { return next_helper_(etl::index_sequence_for<Sequences...>{}); }

    private:
        template <size_t i>
        constexpr bool bool_helper_() const {
            if constexpr (i == sizeof...(Sequences)) return true;
            else return bool(etl::get<i>(sequences)) && bool_helper_<i + 1>();
        }

        template <size_t i>
        constexpr void inc_helper_() {
            if constexpr (i < sizeof...(Sequences)) {
                ++etl::get<i>(sequences);
                inc_helper_<i + 1>();
            }
        }

        template <size_t... i>
        constexpr auto deref_helper_(index_sequence<i...>) {
            return Tuple<decltype(*etl::get<i>(sequences))...> { *etl::get<i>(sequences)... } ;
        }

        template <size_t... i>
        constexpr auto deref_helper_(index_sequence<i...>) const {
            return Tuple<add_const_t<decltype(*etl::get<i>(sequences))>...> { *etl::get<i>(sequences)... } ;
        }

        template <size_t... i>
        constexpr auto next_helper_(index_sequence<i...>) {
            using R = decltype(operator*());
            if (!operator bool()) {
                if constexpr (etl::has_empty_constructor_v<R>) {
                    // avoid segfault
                    return R();
                } else {
                    // segfault
                    return *static_cast<R*>(nullptr);
                }
            }

            auto res = R { *etl::get<i>(sequences)... };
            operator++();
            return res;
        }
    };

    /// create zip object from begin-end pair
    template <typename... Iterators> constexpr auto
    zip(Pair<Iterators>... begin_end) { return etl::Zip(etl::iter(begin_end.x, begin_end.y)...); }

    /// create zip object from any sequence
    template <typename... Sequences> constexpr auto
    zip(Sequences&&... seq) { return etl::Zip(etl::iter(seq)...); }
}

#endif // ETL_ZIP_H
