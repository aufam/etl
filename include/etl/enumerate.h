#ifndef ETL_ENUMERATE_H
#define ETL_ENUMERATE_H

#include "etl/tuple.h"
#include "etl/iter.h"

namespace Project::etl {

    /// python-like enumerate
    template <typename Sequence>
    class Enumerate {
        Sequence sequence;
        int cnt;

    public:
        constexpr explicit Enumerate(Sequence seq, int cnt = 0) : sequence(seq), cnt(cnt) {}
 
        constexpr const Enumerate& begin() const& { return *this; }
        constexpr const Enumerate& end()   const& { return *this; }
        constexpr const Enumerate& iter()  const& { return *this; }

        constexpr Enumerate begin() && { return etl::move(*this); }
        constexpr Enumerate end()   && { return etl::move(*this); }
        constexpr Enumerate iter()  && { return etl::move(*this); }

        constexpr explicit operator bool() const { return bool(sequence); }

        constexpr bool operator!=(const Enumerate&) const { return operator bool(); }
        
        constexpr void operator++() { ++sequence; ++cnt; }

        constexpr auto operator*() { return etl::Tuple<int, decltype(*sequence)>{cnt, *sequence}; }
        constexpr auto operator*() const { return etl::Tuple<int, add_const_t<decltype(*sequence)>>{cnt, *sequence}; }
        
        constexpr auto operator()() {
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

            auto res = operator*();
            operator++();
            return res;
        }
    };

    /// create enumerate object from iterator
    template <typename Iterator> constexpr auto
    enumerate(Iterator first, Iterator last, int cnt = 0, int step = 1) {
        return Enumerate(etl::iter(first, last, step), cnt);
    }

    /// create enumerate object from a container
    template <typename Sequence> constexpr auto
    enumerate(Sequence&& seq, int cnt = 0) { return Enumerate(etl::iter(seq), cnt); }
}

#endif // ETL_ENUMERATE_H