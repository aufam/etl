#ifndef ETL_ASYNC_H
#define ETL_ASYNC_H

#include "etl/future.h"

namespace Project::etl {
    template <typename T> class Async;

    template <typename R, typename... Args>
    class [[nodiscard]] Async<R(Args...)> {
    public:
        template <typename Functor>
        Async(Functor&& f) : fn(std::forward<Functor>(f)) {}

        template <typename... InnerArgs>
        Future<R> operator()(InnerArgs&&... args) const { 
            return [this, args_=std::make_tuple(std::forward<InnerArgs>(args)...)]() mutable {
                return std::apply(fn, std::move(args_));
            };
        }
    
    private:
        std::function<R(Args...)> fn;
    };
}

#endif