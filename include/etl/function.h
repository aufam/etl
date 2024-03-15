#ifndef ETL_FUNCTION_H
#define ETL_FUNCTION_H

#include "etl/algorithm.h"
#include "etl/tuple.h"

namespace Project::etl::detail {
    template <typename, auto>
    struct ExtractMethod;

    template <typename C, typename R, typename... Args, auto method>
    struct ExtractMethod<R (C::*)(Args...), method> { 
        using Class = C;
        using ClassPtr = Class*;
        using Return = R;
        using Fn = Return(Args...);
        using Fp = Return(*)(ClassPtr, Args...);
        static inline Return(*const fn)(ClassPtr, Args...) = +[] (ClassPtr self, Args... args) {
            return (self->*method)(args...);
        };
    };

    template <typename C, typename R, typename... Args, auto method>
    struct ExtractMethod<R (C::*)(Args...) const, method> {
        using Class = C;
        using ClassPtr = const Class*;
        using Return = R;
        using Fn = Return(Args...);
        using Fp = Return(*)(ClassPtr, Args...);
        static inline Return(*const fn)(ClassPtr, Args...) = +[] (ClassPtr self, Args... args) {
            return (self->*method)(args...);
        };
    };
}

namespace Project::etl {
    /// function class that holds function pointer and context (alternative of capture list)
    template <typename F, typename... Ctx> struct Function;

    template <typename T> struct is_etl_function : false_type {};
    template <typename F, typename... Ctx> struct is_etl_function<Function<F, Ctx...>> : true_type {};
    template <typename T> inline constexpr bool is_etl_function_v = is_etl_function<T>::value;

    template <typename R, typename... Args, typename... Ctx>
    struct Function<R(Args...), Ctx...> {
        typedef R Result;
        typedef R (* Fn)(Ctx..., Args...);
        typedef Tuple<Ctx...> Context;

        /// empty constructor
        constexpr Function() : fn(nullptr) {}
        constexpr Function(decltype(nullptr)) : fn(nullptr) {}
        constexpr Function& operator=(decltype(nullptr)) { this->~Function(); return *this; }

        /// construct from a functor that is convertible to a function pointer
        template <typename Functor, typename = disable_if_t<is_etl_function_v<decay_t<Functor>>>>
        constexpr Function(Functor&& fn, Ctx... context) : fn(static_cast<Fn>(etl::forward<Functor>(fn))) {
            new (this->context) Context{etl::forward<Ctx>(context)...};
        }

        /// copy constructor
        constexpr Function(const Function& other) : fn(other.fn) {
            if (fn) new (this->context) Context(*reinterpret_cast<Context*>(other.context));
        }

        /// copy assignment
        constexpr Function& operator=(const Function& other) {
            if (&other == this) return *this;
            this->~Function();
            fn = other.fn;
            if (fn) new (this->context) Context(*reinterpret_cast<Context*>(other.context));
            return *this;
        }

        /// move constructor
        constexpr Function(Function&& other) noexcept : fn(etl::exchange(other.fn, nullptr)) {
            etl::copy(other.context, other);
        }

        /// move assignment
        constexpr Function& operator=(Function&& other) noexcept {
            fn = etl::exchange(other.fn, nullptr);
            etl::copy(other.context, other);
            return *this;
        }

        ~Function() {
            if (fn) {
                reinterpret_cast<Context*>(context)->~Context();
                fn = nullptr;
            }
        }

        /// invoke operator
        R operator()(Args... args) const { return invoke(index_sequence_for<Ctx...>{}, etl::forward<Args>(args)...); }

        /// return true if fn is not null
        explicit constexpr operator bool() const { return fn; }

        /// cast to function pointer
        explicit constexpr operator Fn() const { return fn; }

        constexpr bool operator==(const Function& other) const { return fn == other.fn; }
        constexpr bool operator!=(const Function& other) const { return !operator==(other); }

    private:
        template <size_t... i>
        R invoke(index_sequence<i...>, Args... args) const { 
            if (fn) {
                return fn(etl::get<i>(*reinterpret_cast<Context*>(context))..., etl::forward<Args>(args)...); 
            } else { // undefined behavior
                if constexpr (etl::has_empty_constructor_v<R>) {
                    return R();
                } else {
                    uint8_t dummy[sizeof(R)] = {};
                    return *reinterpret_cast<R*>(dummy);
                }
            }
        }

        Fn fn; ///< function pointer
        mutable uint8_t context[sizeof(Context)] = {}; ///< alternative of capture list in a lambda expression
    };

    /// function class specialization for Context = void
    template <typename R, typename... Args>
    struct Function<R(Args...)> {
        typedef R Result;
        typedef R (* Fn)(Args...);
        typedef void Context;
        friend struct Function<R(Args...), void*>;

        /// empty constructor
        constexpr Function() : fn(nullptr) {}

        constexpr Function(decltype(nullptr)) : fn(nullptr) {}
        constexpr Function& operator=(decltype(nullptr)) { fn = nullptr; return *this; }

        /// construct from a functor that is convertible to a function pointer
        template <typename Functor, typename = disable_if_t<is_etl_function_v<decay_t<Functor>>>>
        constexpr Function(Functor&& fn) : fn(static_cast<Fn>(etl::forward<Functor>(fn))) {}

        /// assign from a functor that is convertible to a function pointer
        template <typename Functor, typename = disable_if_t<is_etl_function_v<decay_t<Functor>>>>
        constexpr Function& operator=(Functor&& f) {
            fn = static_cast<Fn>(etl::forward<Functor>(f)); 
            return *this;
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

        ~Function() { fn = nullptr; }

        /// invoke operator
        constexpr R operator()(Args... args) const { return invoke(etl::forward<Args>(args)...); }

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

    private:
        R invoke(Args... args) const { 
            if (fn) {
                return fn(etl::forward<Args>(args)...); 
            } else { // undefined behavior
                if constexpr (etl::has_empty_constructor_v<R>) {
                    return R();
                } else {
                    uint8_t dummy[sizeof(R)] = {};
                    return *reinterpret_cast<R*>(dummy);
                }
            }
        }

        Fn fn; ///< function pointer, default null
    };

    /// function class specialization for Context = pointer
    template <typename R, typename... Args, typename Ctx>
    struct Function<R(Args...), Ctx*> {
        typedef R Result;
        typedef R (* Fn)(Ctx*, Args...);
        typedef Ctx* Context;
        friend struct Function<R(Args...), void*>;

        /// empty constructor
        constexpr Function() : fn(nullptr), context(nullptr) {}

        constexpr Function(decltype(nullptr)) : Function() {}
        constexpr Function& operator=(decltype(nullptr)) { this->~Function(); return *this; }

        /// construct from a functor that is convertible to a function pointer
        template <typename Functor, typename = disable_if_t<is_etl_function_v<decay_t<Functor>>>>
        constexpr Function(Functor&& fn, Ctx* context) : fn(static_cast<Fn>(etl::forward<Functor>(fn))), context(context) {}

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

        ~Function() { 
            fn = nullptr;
            context = nullptr;
        }

        /// invoke operator
        constexpr R operator()(Args... args) const { return invoke(etl::forward<Args>(args)...); }

        /// return true if fn is not null
        explicit constexpr operator bool() const { return fn; }

        /// cast to function pointer
        explicit constexpr operator Fn() const { return fn; }

        constexpr bool operator==(const Function& other) const { return fn == other.fn; }
        constexpr bool operator!=(const Function& other) const { return !operator==(other); }

    private:
        R invoke(Args... args) const { 
            if (fn) {
                return fn(context, etl::forward<Args>(args)...); 
            } else { // undefined behavior
                if constexpr (etl::has_empty_constructor_v<R>) {
                    return R();
                } else {
                    uint8_t dummy[sizeof(R)] = {};
                    return *reinterpret_cast<R*>(dummy);
                }
            }
        }

        Fn fn; ///< function pointer, default null
        Context context; ///< alternative of capture list, default null
    };
    
    /// function class specialization for Context = reference
    template <typename R, typename... Args, typename Ctx>
    struct Function<R(Args...), Ctx&> {
        typedef R Result;
        typedef R (* Fn)(Ctx&, Args...);
        typedef Ctx* Context;
        friend struct Function<R(Args...), void*>;

        /// empty constructor
        constexpr Function() : fn(nullptr), context(nullptr) {}

        constexpr Function(decltype(nullptr)) : Function() {}
        constexpr Function& operator=(decltype(nullptr)) { this->~Function(); return *this; }

        /// construct from a functor that is convertible to a function pointer
        template <typename Functor, typename = disable_if_t<is_etl_function_v<decay_t<Functor>>>>
        constexpr Function(Functor&& fn, Ctx& context) : fn(static_cast<Fn>(etl::forward<Functor>(fn))), context(&context) {}

        /// copy constructor
        constexpr Function(const Function& other) : fn(other.fn), context(other.context) {}

        /// copy assignment
        constexpr Function& operator=(const Function& other) {
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

        ~Function() { 
            fn = nullptr;
            context = nullptr;
        }

        /// invoke operator
        constexpr R operator()(Args... args) const { return invoke(etl::forward<Args>(args)...); }

        /// return true if fn is not null
        explicit constexpr operator bool() const { return fn; }

        /// cast to function pointer
        explicit constexpr operator Fn() const { return fn; }

        constexpr bool operator==(const Function& other) const { return fn == other.fn; }
        constexpr bool operator!=(const Function& other) const { return !operator==(other); }

    private:
        R invoke(Args... args) const { 
            if (fn) {
                return fn(*context, etl::forward<Args>(args)...); 
            } else { // undefined behavior
                if constexpr (etl::has_empty_constructor_v<R>) {
                    return R();
                } else {
                    uint8_t dummy[sizeof(R)] = {};
                    return *reinterpret_cast<R*>(dummy);
                }
            }
        }

        Fn fn; ///< function pointer, default null
        Context context; ///< alternative of capture list, default null
    };

    /// function class specialization for Context = void*
    template <typename R, typename... Args>
    struct Function<R(Args...), void*> {
        typedef R Result;
        typedef R (* Fn)(void*, Args...);
        typedef R (* Fp)(Args...);
        typedef void* Context;

        /// empty constructor
        constexpr Function() : fn(nullptr), context(nullptr) {}

        constexpr Function(decltype(nullptr)) : Function() {}
        constexpr Function& operator=(decltype(nullptr)) { this->~Function(); return *this; }

        /// construct from a functor that is convertible to a function pointer
        template <typename Functor, typename Ctx, typename = disable_if_t<is_etl_function_v<decay_t<Functor>>>>
        Function(Functor&& f, Ctx* ctx) : Function() {
            auto pf = static_cast<R (*)(Ctx*, Args...)>(etl::forward<Functor>(f));
            fn = reinterpret_cast<Fn>(pf);
            context = reinterpret_cast<Context>(const_cast<etl::remove_const_t<Ctx>*>(ctx));
        }

        /// construct from a functor that is convertible to a function pointer
        template <typename Functor, typename = disable_if_t<is_etl_function_v<decay_t<Functor>> || is_same_v<decay_t<Functor>, None>>>
        Function(Functor&& f) : Function() {
            auto pf = static_cast<R (*)(Args...)>(etl::forward<Functor>(f));
            fn = wrapperFunc;
            context = reinterpret_cast<Context>(pf);
        }

        /// assign from a functor that is convertible to a function pointer
        template <typename Functor, typename = disable_if_t<is_same_v<decay_t<Functor>, Function>>>
        Function& operator=(Functor&& f) {
            auto pf = static_cast<R (*)(Args...)>(etl::forward<Functor>(f));
            fn = wrapperFunc;
            context = reinterpret_cast<Context>(pf);
            return *this;
        }

        /// copy construct from Function<R(Args...)>
        Function(const Function<R(Args...)>& f) : Function() {
            fn = wrapperFunc;
            context = reinterpret_cast<Context>(f.fn);
        }

        /// copy assignment from Function<R(Args...)>
        Function& operator=(const Function<R(Args...)>& other) {
            fn = wrapperFunc;
            context = reinterpret_cast<Context>(other.fn);
            return *this;
        }

        /// move construct from Function<R(Args...)>
        Function(Function<R(Args...)>&& f) : Function() {
            fn = wrapperFunc;
            context = reinterpret_cast<Context>(f.fn);
            f.fn = nullptr;
        }

        /// move assignment from Function<R(Args...)>
        Function& operator=(Function<R(Args...)>&& other) {
            fn = wrapperFunc;
            context = reinterpret_cast<Context>(other.fn);
            other.fn = nullptr;
            return *this;
        }

        /// copy construct from Function<R(Args...), Ctx*>
        template <typename Ctx>
        Function(const Function<R(Args...), Ctx*>& f) : Function() {
            using MutCtx = etl::remove_const_t<Ctx>;
            fn = reinterpret_cast<Fn>(f.fn);
            context = reinterpret_cast<Context>(const_cast<MutCtx*>(f.context));
        }

        /// copy assignment from Function<R(Args...), Ctx*>
        template <typename Ctx>
        Function& operator=(const Function<R(Args...), Ctx*>& other) {
            using MutCtx = etl::remove_const_t<Ctx>;
            fn = reinterpret_cast<Fn>(other.fn);
            context = reinterpret_cast<Context>(const_cast<MutCtx*>(other.context));
            return *this;
        }

        /// move construct from Function<R(Args...), Ctx*>
        template <typename Ctx>
        Function(Function<R(Args...), Ctx*>&& f) : Function() {
            using MutCtx = etl::remove_const_t<Ctx>;
            fn = reinterpret_cast<Fn>(f.fn);
            context = reinterpret_cast<Context>(const_cast<MutCtx*>(f.context));
            f.fn = nullptr;
            f.context = nullptr;
        }

        /// move assignment from Function<R(Args...), Ctx*>
        template <typename Ctx>
        Function& operator=(Function<R(Args...), Ctx*>&& other) {
            using MutCtx = etl::remove_const_t<Ctx>;
            fn = reinterpret_cast<Fn>(other.fn);
            context = reinterpret_cast<Context>(const_cast<MutCtx*>(other.context));
            other.fn = nullptr;
            other.context = nullptr;
            return *this;
        }

        /// copy construct from Function<R(Args...), Ctx&>
        template <typename Ctx>
        Function(const Function<R(Args...), Ctx&>& f) : Function() {
            using MutCtx = etl::remove_const_t<Ctx>;
            fn = reinterpret_cast<Fn>(f.fn);
            context = reinterpret_cast<Context>(const_cast<MutCtx*>(f.context));
        }

        /// copy assignment from Function<R(Args...), Ctx&>
        template <typename Ctx>
        Function& operator=(const Function<R(Args...), Ctx&>& other) {
            using MutCtx = etl::remove_const_t<Ctx>;
            fn = reinterpret_cast<Fn>(other.fn);
            context = reinterpret_cast<Context>(const_cast<MutCtx*>(other.context));
            return *this;
        }

        /// move construct from Function<R(Args...), Ctx&>
        template <typename Ctx>
        Function(Function<R(Args...), Ctx&>&& f) : Function() {
            using MutCtx = etl::remove_const_t<Ctx>;
            fn = reinterpret_cast<Fn>(f.fn);
            context = reinterpret_cast<Context>(const_cast<MutCtx*>(f.context));
            f.fn = nullptr;
            f.context = nullptr;
        }

        /// move assignment from Function<R(Args...), Ctx&>
        template <typename Ctx>
        Function& operator=(Function<R(Args...), Ctx&>&& other) {
            using MutCtx = etl::remove_const_t<Ctx>;
            fn = reinterpret_cast<Fn>(other.fn);
            context = reinterpret_cast<Context>(const_cast<MutCtx*>(other.context));
            other.fn = nullptr;
            other.context = nullptr;
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

        ~Function() { 
            fn = nullptr;
            context = nullptr;
        }

        /// invoke operator
        constexpr R operator()(Args... args) const { return invoke(etl::forward<Args>(args)...); }

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
        R invoke(Args... args) const { 
            if (fn) {
                return fn(context, etl::forward<Args>(args)...); 
            } else { // undefined behavior
                if constexpr (etl::has_empty_constructor_v<R>) {
                    return R();
                } else {
                    uint8_t dummy[sizeof(R)] = {};
                    return *reinterpret_cast<R*>(dummy);
                }
            }
        }

        static R wrapperFunc(Context f, Args... args) {
            auto pf = reinterpret_cast<R (*)(Args...)>(f);
            if (pf) {
                return pf(etl::forward<Args>(args)...); 
            } else { // undefined behavior
                if constexpr (etl::has_empty_constructor_v<R>) {
                    return R();
                } else {
                    uint8_t dummy[sizeof(R)] = {};
                    return *reinterpret_cast<R*>(dummy);
                }
            }
        }

        Fn fn = nullptr; ///< function pointer, default null
        Context context = nullptr; ///< alternative of capture list, default null
    };

    /// create empty function, function and context are null
    template <typename F> constexpr auto
    function() { return Function<F, void*>(); }

    /// create function with no context
    template <typename F, typename Fn> constexpr auto
    function(Fn&& fn) { return Function<F>(etl::forward<Fn>(fn)); }

    /// create function with context
    template <typename F, typename Context, typename Fn> auto
    function(Fn&& fn, Context* ctx) { return Function<F, void*>(etl::forward<Fn>(fn), ctx); }

    /// create function from function pointer with no context
    template <typename R, typename... Args> constexpr auto
    function(R (*fn)(Args...)) { return Function<R(Args...)>(fn); }

    /// create function with context
    template <typename ...C, typename R> auto
    function(R (*fn)(C...), C... context) { return Function<R(), C...>(fn, etl::forward<C>(context)...); }

    /// create function with context, capture by reference
    template <typename ...C, typename R> auto
    functionR(R (*fn)(C&...), C&... context) { return Function<R(), C&...>(fn, context...); }

    /// bind a class method to etl function
    template <auto method> constexpr auto
    bind(typename detail::ExtractMethod<decltype(method), method>::ClassPtr self) {
        using Extractor = detail::ExtractMethod<decltype(method), method>;
        return Function<typename Extractor::Fn, typename Extractor::ClassPtr> { Extractor::fn, self };
    }
}

#endif //ETL_FUNCTION_H
