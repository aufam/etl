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

        Future(etl::Function<etl::Result<T>(etl::Time), void*>&& static_fn) : static_fn(etl::move(static_fn)) {}

        template <typename F, typename = etl::enable_if_t<
            etl::is_same_v<etl::Result<T>, decltype(etl::declval<F>()(etl::declval<etl::Time>()))>
        >> Future(F&& fn) : fn(etl::forward<F>(fn)) {}

        Future(Future&& other) {
            *this = etl::move(other);
        }

        Future& operator=(Future&& other) {
            static_fn = etl::move(other.static_fn);
            fn = etl::move(other.fn);
            result = etl::move(result);
        }

        T await() { 
            wait(etl::time::infinite);
            return get();
        }

        osStatus_t wait(etl::Time timeout) {
            result = fn ? fn(timeout) : static_fn(timeout);
            fn = nullptr;
            static_fn = nullptr;
            return result.status;
        }

        T get() {
            if (valid())
                wait(etl::time::infinite);
            
            if constexpr (!etl::is_void_v<T>) {
                return result;
            }
        }

        bool valid() const {
            return fn || static_fn;
        }

        template <typename F, typename R = decltype(etl::declval<F>()(etl::declval<T>()))>
        Future<R> then(F&& other) {
            return [future=etl::move(*this), other=etl::forward<F>(other)] (etl::Time timeout) -> Result<R> {
                auto status = future.wait(timeout);
                if constexpr (etl::is_void_v<R>) {
                    return status;
                } else {
                    if (status == osOK) {
                        return future.get();
                    } else {
                        return status;
                    }
                }
            };
        }

    private:
        etl::Function<Result<T>(etl::Time), void*> static_fn;
        std::function<Result<T>(etl::Time)> fn;
        etl::Result<T> result = osError;
    };
}

#endif