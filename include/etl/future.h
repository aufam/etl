#ifndef ETL_FUTURE_H
#define ETL_FUTURE_H

#include "etl/function.h"
#include <functional>

namespace Project::etl {
    class Tasks;

    template <typename T>
    class Future {
        friend class Tasks;

    public:
        Future(etl::Function<T(), void*>&& f) : fn_static(etl::move(f)) {}

        template <typename Functor, typename = disable_if_t<is_etl_function_v<Functor>>>
        Future(Functor&& f) : fn(etl::forward<Functor>(f)) {}

        T await() const { return fn ? fn() : fn_static(); }

        template <typename Functor>
        auto then(Functor&& func) {
            if constexpr (etl::is_void_v<T>)
            return Future<decltype(func())>([future=etl::move(*this), func=etl::forward<Functor>(func)] { 
                return (future.await(), func()); 
            });
            else
            return Future<decltype(func(etl::declval<T>()))>([future=etl::move(*this), func=etl::forward<Functor>(func)] { 
                return func(future.await()); 
            });
        } 

        template <typename U>
        Future<U> operator|(Future<U>&& other) {
            return [future=etl::move(*this), other=etl::move(other)] { return (future.await(), other.await()); };
        } 

    private:
        etl::Function<T(), void*> fn_static;
        std::function<T()> fn;
    };
}

#endif