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

        T wait() const { return fn(); }

        template <typename Functor>
        auto then(Functor&& func) {
            if constexpr (etl::is_void_v<T>)
            return Future<decltype(func())>([get=etl::move(fn), func=etl::forward<Functor>(func)] { 
                return (get(), func()); 
            });
            else
            return Future<decltype(func(etl::declval<T>()))>([get=etl::move(fn), func=etl::forward<Functor>(func)] { 
                return func(get()); 
            });
        } 

        template <typename U>
        Future<U> operator|(Future<U>&& future) {
            return [get=etl::move(fn), future=etl::move(future)] { return (get(), future.fn()); };
        } 

        std::future<T> launch() {
            return std::async(etl::move(fn));
        }

    private:
        std::function<T()> fn;
    };

    inline static constexpr struct Await {
        template <typename T>
        T operator|(const Future<T>& future) const { return future.wait(); }
    } await;
}

#endif