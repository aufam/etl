#ifndef ETL_FUTURE_H
#define ETL_FUTURE_H

#include "etl/utility.h"
#include <functional>
#include <future>

namespace Project::etl {

    template <typename T>
    class [[nodiscard]] Future {
    public:
        template <typename Functor>
        Future(Functor&& f) : fn(etl::forward<Functor>(f)) {}

        T await() const { return fn(); }

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

        std::future<T> launch() {
            return std::async(etl::move(fn));
        }

    private:
        std::function<T()> fn;
    };
}

#endif
