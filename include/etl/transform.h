#ifndef ETL_TRANSFORM_H
#define ETL_TRANSFORM_H

#include "etl/iter.h"

namespace Project::etl {

    template <typename Sequence, typename UnaryFunction>
    class Transform {
        Sequence sequence;
        UnaryFunction fn;

    public:
        constexpr Transform(Sequence sequence, UnaryFunction fn) : sequence(sequence), fn(fn) {}

        constexpr const Transform& begin() const& { return *this; }
        constexpr const Transform& end()   const& { return *this; }
        constexpr const Transform& iter()  const& { return *this; }

        constexpr Transform&& begin() && { return etl::move(*this); }
        constexpr Transform&& end()   && { return etl::move(*this); }
        constexpr Transform&& iter()  && { return etl::move(*this); }

        constexpr explicit operator bool() const { return bool(sequence); }

        constexpr bool operator!=(const Transform&) const { return operator bool(); }
        
        constexpr void operator++() { ++sequence; }

        constexpr decltype(auto) operator*() { return fn(*sequence); }

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

            decltype(auto) res = operator*(); // return type: int
            operator++();
            return res; // inconsistent deduction for auto return type: ‘int&’ and then ‘int’
        }
    };

    /// create transform object from iterator
    template <typename Iterator, typename UnaryFunction> constexpr auto
    transform(Iterator first, Iterator last, UnaryFunction&& fn, int step = 1) { return Transform(etl::iter(first, last, step), etl::forward<UnaryFunction>(fn)); }

    /// create transform object from a sequence
    template <typename Sequence, typename UnaryFunction> constexpr auto
    transform(Sequence&& seq, UnaryFunction&& fn) { return Transform(etl::iter(seq), etl::forward<UnaryFunction>(fn)); }

    template <typename UnaryFunction>
    struct TransformFunction { UnaryFunction fn; };

    /// create transform function
    template <typename UnaryFunction> constexpr auto
    transform(UnaryFunction&& fn) { return TransformFunction<UnaryFunction> { etl::forward<UnaryFunction>(fn) }; }

    /// pipe operator to apply transform function to a sequence
    template <typename Sequence, typename UnaryFunction> constexpr auto
    operator|(Sequence&& seq, TransformFunction<UnaryFunction> fn) { return etl::transform(etl::forward<Sequence>(seq), etl::move(fn.fn)); }

    /// remove extent
    template <typename Sequence, typename UnaryFunction>
    struct remove_extent<Transform<Sequence, UnaryFunction>> {
        typedef decltype(etl::next<Transform<Sequence, UnaryFunction>>(etl::declval<Transform<Sequence, UnaryFunction>>())) type;
    };

    template <typename Sequence, typename UnaryFunction>
    struct remove_extent<const Transform<Sequence, UnaryFunction>> {
        typedef remove_extent_t<Transform<Sequence, UnaryFunction>> type;
    };

    template <typename Sequence, typename UnaryFunction>
    struct remove_extent<volatile Transform<Sequence, UnaryFunction>> {
        typedef remove_extent_t<Transform<Sequence, UnaryFunction>> type;
    };

    template <typename Sequence, typename UnaryFunction>
    struct remove_extent<const volatile Transform<Sequence, UnaryFunction>> {
        typedef remove_extent_t<Transform<Sequence, UnaryFunction>> type;
    };
} 

#endif // ETL_TRANSFORM_H
