#ifndef ETL_UTILITY_BASIC_H
#define ETL_UTILITY_BASIC_H

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

    /// exchange the values of a and b 
    template <class T> constexpr void
    swap(T& a, T& b) {
        T temp(etl::move(a));
        a = etl::move(b);
        b = etl::move(temp);
    }

    /// replace the value of obj with new_value and returns the old value of obj
    template <typename T, typename U = T> constexpr T 
    exchange(T& obj, U&& new_value) {
        T old = etl::move(obj);
        obj = etl::forward<U>(new_value);
        return old;
    }

    /// pointer to first element
    template <typename Container> constexpr auto
    begin(Container& cont) -> decltype(cont.begin()) { return cont.begin(); }
    
    template <typename Container> constexpr auto
    begin(const Container& cont) -> decltype(cont.begin()) { return cont.begin(); }
    
    template <typename Container> constexpr auto
    begin(Container&& cont) -> decltype(cont.begin()) { return cont.begin(); }
    
    template <typename Container> constexpr auto
    begin(const Container&& cont) -> decltype(cont.begin()) { return cont.begin(); }
    
    template <typename T, size_t N> constexpr T*
    begin(T (&arr)[N]) { return arr; }

    /// pointer to one past the last element
    template <typename Container> constexpr auto
    end(Container& cont) -> decltype(cont.end()) { return cont.end(); }

    template <typename Container> constexpr auto
    end(const Container& cont) -> decltype(cont.end()) { return cont.end(); }
    
    template <typename Container> constexpr auto
    end(Container&& cont) -> decltype(cont.end()) { return cont.end(); }
    
    template <typename Container> constexpr auto
    end(const Container&& cont) -> decltype(cont.end()) { return cont.end(); }
    
    template <typename T, size_t N> constexpr T*
    end(T (&arr)[N]) { return arr + N; }

    /// python-like len
    template <typename Container> constexpr auto
    len(Container& cont) -> decltype(cont.len()) { return cont.len(); }
    
    template <typename Container> constexpr auto
    len(const Container& cont) -> decltype(cont.len()) { return cont.len(); }
    
    template <typename Container> constexpr auto
    len(Container&& cont) -> decltype(cont.len()) { return cont.len(); }
    
    template <typename Container> constexpr auto
    len(const Container&& cont) -> decltype(cont.len()) { return cont.len(); }
    
    template <typename T, size_t N> constexpr auto
    len(T (&)[N]) { return N; }
    
    template <typename T> constexpr auto
    len(std::initializer_list<T> list) { return list.size(); }
    
    template <typename T> constexpr auto
    len(std::initializer_list<T>&& list) { return list.size(); }

    /// python-like next
    template <typename Sequence> constexpr decltype(auto)
    next(Sequence& gen) { return gen(); }

    /// python-like next
    template <typename Sequence> constexpr decltype(auto)
    next(Sequence&& gen) { return gen(); }

    /// get the address of an object
    template <typename T> constexpr T*
    addressof(T& x) { return &x; }

    /// constant rvalue reference does not have an address
    template<class T> void
    addressof(const T&&) = delete;

    /// none type
    struct None {
        template <typename T, typename = enable_if_t<etl::is_compound_v<T>>>
        constexpr operator T() const { return T{}; } // NOLINT
    };
    inline static constexpr None none;

    template <typename T, typename = enable_if_t<etl::is_compound_v<T>>> constexpr bool
    operator==(const T& value, None) {
        if constexpr (has_empty_v<T>) return value.empty();
        else return !bool(value);
    }

    template <typename T, typename = enable_if_t<etl::is_compound_v<T>>> constexpr bool
    operator==(None, const T& value) { return operator==(value, etl::none); }

    template <typename T, typename = enable_if_t<etl::is_compound_v<T>>> constexpr bool
    operator!=(const T& value, None) { return !operator==(value, etl::none); }

    template <typename T, typename = enable_if_t<etl::is_compound_v<T>>> constexpr bool
    operator!=(None, const T& value) { return !operator==(value, etl::none); }

    inline static constexpr struct {
        template <typename T>
        constexpr void operator=(T&&) const {}
    } ignore;

    inline static constexpr struct {
        template <typename T>
        constexpr auto operator|(T&& o) const { return etl::move(o); }
    } mv;

    inline static constexpr struct {
        template <typename T>
        constexpr auto operator|(T&& o) const { return o; }
    } cp;

    inline static constexpr struct {
        template <typename T>
        auto operator|(T&& o) const { return o.await(); }
    } await;
    
    /// manages cleanup actions on scope exit using RAII principles.
    template <typename Callable>
    class [[nodiscard]] Defer {
        Callable onExit;
    
    public:
        Defer(Callable&& onExit) : onExit(etl::forward<Callable>(onExit)) {}
        ~Defer() { onExit(); }

        Defer(const Defer&) = delete;             // No copy constructor
        Defer& operator=(const Defer&) = delete;  // No copy assignment
        Defer(Defer&&) = delete;                  // No move constructor
        Defer& operator=(Defer&&) = delete;       // No move assignment
    };

    inline static constexpr struct {
        template <typename F>
        auto operator|(F&& fn) const { return Defer<etl::decay_t<F>>(etl::forward<F>(fn)); }
    } defer;
}

#endif // ETL_UTILITY_BASIC_H
