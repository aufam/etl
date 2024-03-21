#ifndef ETL_FUTURE_H
#define ETL_FUTURE_H

#include "etl/time.h"
#include "etl/function.h"
#include "etl/result.h"
#include <functional>

namespace Project::etl {
    template <typename T>
    class Future {
    public:
        Future() {}

        template <typename Ctx>
        Future(Function<Result<T, osStatus_t>(Time), Ctx*>&& static_fn) : static_fn(etl::move(static_fn)) {}

        template <typename F, typename = etl::enable_if_t<
            etl::is_same_v<etl::Result<T, osStatus_t>, decltype(etl::declval<F>()(etl::declval<Time>()))>
        >> Future(F&& fn) : fn(etl::forward<F>(fn)) {}

        Result<T, osStatus_t> await() { 
            return wait(time::infinite);
        }

        Result<T, osStatus_t> wait(Time timeout) {
            if (!valid())
                return Err(osError);
            
            auto res = fn ? fn(timeout) : static_fn(timeout);
            fn = nullptr;
            static_fn = nullptr;
            return res;
        }

        bool valid() const {
            return fn || static_fn;
        }

    private:
        etl::Function<Result<T, osStatus_t>(Time), void*> static_fn;
        std::function<Result<T, osStatus_t>(Time)> fn;
    };
}

#endif