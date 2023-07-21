#ifndef ETL_FUNCTION_H
#define ETL_FUNCTION_H

#include "etl/algorithm.h"
#include "etl/tuple.h"
#include <thread>

namespace Project::etl {
    /// function class that holds function pointer and context (alternative of capture list)
    template <typename T, typename... C> struct Function;

    template <typename R, typename... Args, typename... C>
    struct Function<R(Args...), C...> {
        typedef R Result;
        typedef R (* Fn)(C..., Args...);
        typedef Tuple<C...> Context;

        Fn fn;                   ///< function pointer
        mutable Context context; ///< alternative of capture list in a lambda expression
        
        /// construct from a functor (capture-less lambda expression, function pointer, or other invokable object)
        template <typename Functor, typename = disable_if_t<is_same_v<decay_t<Functor>, Function>>>
        constexpr Function(Functor&& fn, C... context)
        : fn(static_cast<Fn>(etl::forward<Functor>(fn)))
        , context{context...} {}

        /// assign from a functor (capture-less lambda expression, function pointer, or other invokable object)
        template <typename Functor, typename = disable_if_t<is_same_v<decay_t<Functor>, Function>>>
        Function& operator=(Functor&& f) {
            fn = static_cast<Fn>(etl::forward<Functor>(f)); return *this; 
        }

        /// copy constructor
        constexpr Function(const Function& other) : fn(other.fn), context(other.context) {}

        /// copy assignment
        constexpr Function& operator=(const Function& other) {
            if (&other == this) return *this;
            fn = other.fn;
            context = other.context;
            return *this;
        }

        /// move constructor
        constexpr Function(Function&& other) noexcept : fn(etl::move(other.fn)), context(etl::move(other.context)) {
            other.fn = nullptr; 
        }
        
        /// move assignment
        constexpr Function& operator=(Function&& other) noexcept {
            fn = etl::move(other.fn);
            context = etl::move(other.context);
            other.fn = nullptr;
            return *this;
        }

        /// invoke operator
        constexpr R operator()(Args... args) const { return fn ? invoke(index_sequence_for<C...>{}, args...) : R(); }

        /// return true if fn is not null
        explicit constexpr operator bool() const { return fn; }

        /// cast to function pointer
        explicit constexpr operator Fn() const { return fn; }

        constexpr bool operator==(const Function& other) const { return fn == other.fn && context == other.context; }
        constexpr bool operator!=(const Function& other) const { return !operator==(other); }

    private:
        template <size_t... i>
        constexpr decltype(auto) invoke(index_sequence<i...>, Args... args) const { return fn(etl::get<i>(context)..., args...); }
    };

    /// function class specialization for Context = void
    template <typename R, typename... Args>
    struct Function<R(Args...)> {
        typedef R Result;
        typedef R (* Fn)(Args...);
        typedef void Context;

        Fn fn; ///< function pointer, default null
        
        /// empty constructor
        constexpr Function() : fn(nullptr) {}

        /// construct from a functor (capture-less lambda expression, function pointer, or other invokable object)
        template <typename Functor, typename = disable_if_t<is_same_v<decay_t<Functor>, Function>>>
        constexpr Function(Functor&& fn)
        : fn(static_cast<Fn>(etl::forward<Functor>(fn))) {}

        /// assign from a functor (capture-less lambda expression, function pointer, or other invokable object)
        template <typename Functor, typename = disable_if_t<is_same_v<decay_t<Functor>, Function>>>
        constexpr Function& operator=(Functor&& f) {
            fn = static_cast<Fn>(etl::forward<Functor>(f)); return *this; 
        }

        /// copy constructor
        constexpr Function(const Function& other) : fn(other.fn) {}

        /// copy assignment
        constexpr Function& operator=(const Function& other) {
            if (&other == this) return *this;
            fn = other.fn;
            return *this;
        }

        /// move constructor
        constexpr Function(Function&& other) noexcept : fn(etl::exchange(other.fn, nullptr)) {}
        
        /// move assignment
        constexpr Function& operator=(Function&& other) noexcept {
            if (&other == this) return *this;
            fn = etl::exchange(other.fn, nullptr);
            return *this;
        }

        /// invoke operator
        constexpr R operator()(Args... args) const { return fn ? fn(args...) : R(); }

        /// return true if fn is not null
        explicit constexpr operator bool() const { return fn; }

        /// cast to function pointer
        explicit constexpr operator Fn() const { return fn; }

        /// compare operator
        template <typename Functor>
        constexpr bool operator==(Functor&& other) const { 
            if constexpr (etl::is_same_v<etl::decay_t<Functor>, Function<R(Args...), void*>>)
                return other == *this;
            else 
                return fn == static_cast<Fn>(etl::forward<Functor>(other)); 
        }

        template <typename Functor>
        constexpr bool operator!=(Function&& other) const { return !operator==(etl::forward<Functor>(other)); }
    };

    /// function class specialization for Context = void*
    template <typename R, typename... Args>
    struct Function<R(Args...), void*> {
        typedef R Result;
        typedef R (* Fn)(void*, Args...);
        typedef R (* Fp)(Args...);
        typedef void* Context;

        Fn fn;           ///< function pointer, default null
        Context context; ///< alternative of capture list, default null
    
        /// empty constructor
        constexpr Function() : fn(nullptr), context(nullptr) {}

        /// construct from a functor (capture-less lambda expression, function pointer, or other invokable object)
        template <typename Functor, typename Ctx, typename = disable_if_t<is_same_v<decay_t<Functor>, Function>>>
        Function(Functor&& f, Ctx* ctx)
        : fn(nullptr), context(reinterpret_cast<Context>(ctx)) {
            auto pf = static_cast<R (*)(Ctx*, Args...)>(etl::forward<Functor>(f));
            fn = reinterpret_cast<Fn>(pf); 
        }

        /// construct from a functor (capture-less lambda expression, function pointer, or other invokable object)
        template <typename Functor, typename = disable_if_t<is_same_v<decay_t<Functor>, Function>>>
        Function(Functor&& f) : fn(nullptr), context(nullptr) {
            auto pf = static_cast<R (*)(Args...)>(etl::forward<Functor>(f));
            fn = wrapperFunc;
            context = reinterpret_cast<Context>(pf);
        }

        /// assign from a functor (capture-less lambda expression, function pointer, or other invokable object)
        template <typename Functor, typename = disable_if_t<is_same_v<decay_t<Functor>, Function>>>
        Function& operator=(Functor&& f) {
            auto pf = static_cast<R (*)(Args...)>(etl::forward<Functor>(f));
            fn = wrapperFunc;
            context = reinterpret_cast<Context>(pf);
            return *this; 
        }

        /// copy constructor
        constexpr Function(const Function& other) : fn(other.fn), context(other.context) {}

        /// copy assignment
        constexpr Function& operator=(const Function& other) {
            if (&other == this) return *this;
            fn = other.fn;
            context = other.context;
            return *this;
        }

        /// move constructor
        constexpr Function(Function&& other) noexcept
            : fn(etl::exchange(other.fn, nullptr))
            , context(etl::exchange(other.context, nullptr)) {}
        
        /// move assignment
        constexpr Function& operator=(Function&& other) noexcept {
            if (&other == this) return *this;
            fn = etl::exchange(other.fn, nullptr);
            context = etl::exchange(other.context, nullptr);
            return *this;
        }

        /// invoke operator
        constexpr R operator()(Args... args) const { return fn ? fn(context, args...) : R(); }

        /// return true if fn is not null
        explicit constexpr operator bool() const { return fn; }

        /// cast to function pointer
        explicit constexpr operator Fn() const { return fn; }

        /// cast to function pointer
        explicit constexpr operator Fp() const { return fn == wrapperFunc ? reinterpret_cast<Fp>(context) : nullptr; }

        /// cast to context-less function
        explicit constexpr operator Function<R(Args...)>() const { return Fp(*this); }

        template <typename Functor>
        constexpr bool operator==(Functor&& other) const { 
            if constexpr (etl::is_same_v<etl::decay_t<Functor>, Function>)
                return fn == other.fn && context == other.context;
            else 
                return operator==(Function(etl::forward<Functor>(other)));
        }

        constexpr bool operator!=(const Function& other) { return !operator==(other); }

    private:
        static R wrapperFunc(Context f, Args... args) {
            auto pf = reinterpret_cast<R (*)(Args...)>(f);
            return f ? pf(args...) : R();
        }
    };

    /// create empty function, function and context are null
    template <typename T> constexpr auto
    function() { return Function<T, void*>(); }

    /// create function with no context
    template <typename T, typename Fn> constexpr auto
    function(Fn&& fn) { return Function<T>(etl::forward<Fn>(fn)); }
     
    /// create function with context
    template <typename T, typename Context, typename Fn> auto
    function(Fn&& fn, Context* ctx) { return Function<T, void*>(etl::forward<Fn>(fn), ctx); }
    
    /// create function from function pointer with no context
    template <typename R, typename... Args> constexpr auto
    function(R (*fn)(Args...)) { return Function<R(Args...)>(fn); }

    /// create function with context
    template <typename ...C, typename R> auto
    function(R (*fn)(C...), C... context) { return Function<R(), C...>(fn, context...); }

    /// create function with context, capture by reference
    template <typename ...C, typename R> auto
    functionR(R (*fn)(C&...), C&... context) { return Function<R(), C&...>(fn, context...); }
}

#endif //ETL_FUNCTION_H
