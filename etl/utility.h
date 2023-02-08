#ifndef ETL_UTILITY_H
#define ETL_UTILITY_H

namespace Project::etl {

    /// @defgroup Tuple @{
    /// contains the actual value for one item in the tuple.
    /// @tparam i allows the get function to find the value in O(1) time
    template <size_t i, typename Item> struct xTupleLeaf { Item item; };

    /// acts as proxy for the final class that has an extra template parameter `i`.
    template <size_t i, typename... Items> struct xTupleImpl;

    /// base case: empty tuple
    template <size_t i> struct xTupleImpl<i> {};

    /// recursive specialization
    template <size_t i, typename HeadItem, typename... TailItems>
    struct xTupleImpl<i, HeadItem, TailItems...>
            : public xTupleLeaf<i, HeadItem>
            , public xTupleImpl<i + 1, TailItems...> {};


    /// obtain a reference to i-th item in a tuple
    template <size_t i, typename HeadItem, typename... TailItems>
    auto& get(xTupleImpl<i, HeadItem, TailItems...>& tuple) {
        return tuple.xTupleLeaf<i, HeadItem>::item;
    }
    template <size_t i, typename HeadItem, typename... TailItems>
    constexpr auto& get(const xTupleImpl<i, HeadItem, TailItems...>& tuple) {
        return tuple.xTupleLeaf<i, HeadItem>::item;
    }

    /// implements fixed size container, which holds elements of possibly different types
    template <typename... Items> using Tuple = xTupleImpl<0, Items...>;
    /// @}

    /// implements binary tuple, i.e. a pair of values
    template <class X, class Y = X> struct Pair { X x; Y y; };

    /// @defgroup Function @{
    /// proxy structure
    template <typename T> struct Function;

    /// base function structure
    template <class R, class... Args>
    struct Function<R(Args...)> {
        typedef R (* Fn)(void*, Args...);
        Fn fn;                  ///< function pointer
        void *arg = nullptr;    ///< function argument. alternative of capture list

        constexpr R operator ()(Args... args) const { if (fn) return fn(arg, args...); return R{};}
        explicit constexpr operator bool () const { return fn; }
    };

    /// void specialization
    template <class... Args>
    struct Function<void(Args...)> {
        typedef void (* Fn)(void*, Args...);
        Fn fn;                  ///< function pointer
        void *arg = nullptr;    ///< function argument. alternative of capture list

        constexpr void operator ()(Args... args) const { if (fn) fn(arg, args...);}
        explicit constexpr operator bool () const { return fn; }
    };
    /// @}
}

#endif //ETL_UTILITY_H
