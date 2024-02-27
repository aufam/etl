#ifndef ETL_ASYNC_H
#define ETL_ASYNC_H

#include "etl/future.h"

namespace Project::etl {
    template <typename T> class Async;

    template <typename R, typename... Args>
    class [[nodiscard]] Async<R(Args...)> {
        friend class Tasks;
        
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
        template <typename... InnerArgs>
        void assign_args(InnerArgs... args) const { 
            new (_args) std::tuple<Args...>(std::forward<InnerArgs>(args)...);
        }

        R invoke() const { 
            return std::apply(fn, move_tuple_elements(*reinterpret_cast<std::tuple<Args...>*>(_args)));
        }

        static std::tuple<Args...> move_tuple_elements(std::tuple<Args...>& src_tuple) {
            return std::apply([](auto&&... args) {
                return std::make_tuple(std::move(args)...);
            }, std::move(src_tuple));
        }

        std::function<R(Args...)> fn;
        mutable uint8_t _args[sizeof(std::tuple<Args...>)] = {};
    };

}

#endif