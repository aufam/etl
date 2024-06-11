#ifndef ETL_FUTURE_H
#define ETL_FUTURE_H

#include "etl/time.h"
#include "etl/function.h"
#include "etl/result.h"
#include <functional>

namespace Project::etl::detail {
    inline Result<void, osStatus_t> flagsToStatus(uint32_t flags) {
        switch (flags) {
            case osFlagsErrorTimeout:   return Err(osErrorTimeout);
            case osFlagsErrorResource:  return Err(osErrorResource);
            case osFlagsErrorParameter: return Err(osErrorParameter);
            case osFlagsErrorISR:       return Err(osErrorISR);
            default: 
                if (flags & osFlagsError) return Err(osError); 
                else break;
        }

        return Ok();
    }
}

namespace Project::etl {
    
    /// Lazy async 
    template <typename T>
    class Future {
    public:
        /// empty constructor
        Future() {}

        /// construct future function statically
        /// @param static_fn etl::Function with signature `Result<T, osStatus_t>(Time)`
        template <typename Ctx>
        Future(Function<Result<T, osStatus_t>(Time), Ctx*>&& static_fn) : static_fn(etl::move(static_fn)) {}

        /// construct future function dynamically
        /// @param fn callable with signature `Result<T, osStatus_t>(Time)`
        template <typename F, typename = etl::enable_if_t<
            etl::is_same_v<Result<T, osStatus_t>, decltype(etl::declval<F>()(etl::declval<Time>()))>
        >> Future(F&& fn) : fn(etl::forward<F>(fn)) {}

        /// check if this future object is valid
        bool valid() const {
            return fn || static_fn;
        }

        /// wait the result indefinitely
        Result<T, osStatus_t> await() { 
            return wait(time::infinite);
        }

        /// wait the result until timeout
        Result<T, osStatus_t> wait(Time timeout) {
            if (!valid())
                return Err(osError);
            
            auto res = fn ? fn(timeout) : static_fn(timeout);
            fn = nullptr;
            static_fn = nullptr;
            return res;
        }

        /// @brief execute fn before waiting the result
        /// @param fn callable with signature `void()`
        /// @return new future object with result T
        template <typename F>
        Future<T> setup(F&& fn) && {
            return [future=etl::move(*this), fn=etl::forward<F>(fn)](Time timeout) mutable {
                fn();
                return future.wait(timeout);
            };
        }

        /// @brief execute fn after waiting the result
        /// @param fn callable with signature `void()`
        /// @return new future object with result T
        template <typename F>
        Future<T> finally(F&& fn) && {
            return [future=etl::move(*this), fn=etl::forward<F>(fn)](Time timeout) mutable {
                return future.wait(timeout).finally(etl::forward<F>(fn));
            };
        }

        /// @brief chain the result value after waiting the result
        /// @param fn callable with signature `R(T)`
        /// @return new future object with result R
        template <typename F, typename R = decltype(etl::declval<F>()(etl::declval<T>()))>
        Future<R> then(F&& fn) && {
            return [future=etl::move(*this), fn=etl::forward<F>(fn)](Time timeout) mutable {
                return future.wait(timeout).then(etl::forward<F>(fn));
            };
        }

        /// @brief chain the result value after waiting the result
        /// @param fn callable with signature `Result<R, osStatus_t>(T)`
        /// @return new future object with result R
        template <typename F, typename R = decltype(etl::declval<F>()(etl::declval<T>())), 
            typename = etl::enable_if_t<etl::is_etl_result_v<R>>
        > Future<etl::result_value_t<R>> and_then(F&& fn) && {
            return [future=etl::move(*this), fn=etl::forward<F>(fn)](Time timeout) mutable {
                return future.wait(timeout).and_then(etl::forward<F>(fn));
            };
        }

        /// @brief chain the result value after waiting the result
        /// @param fn callable with signature `Result<R, osStatus_t>(Time, T)`
        /// @return new future object with result R
        template <typename F, typename R = decltype(etl::declval<F>()(etl::declval<Time>(), etl::declval<T>())), 
            typename = etl::enable_if_t<etl::is_etl_result_v<R>>
        > Future<etl::result_value_t<R>> and_async(F&& fn) && {
            return [future=etl::move(*this), fn=etl::forward<F>(fn)](Time timeout) mutable {
                return future.wait(timeout).and_then([fn=etl::forward<F>(fn), timeout](T res) mutable {
                    return fn(timeout, etl::move(res));
                });
            };
        }

        /// @brief chain the result error after waiting the result
        /// @param fn callable with signature `void(osStatus_t)`
        /// @return new future object with result R
        template <typename F, typename = decltype(etl::declval<F>()(etl::declval<osStatus_t>()))>
        Future<T> except(F&& fn) && {
            return [future=etl::move(*this), fn=etl::forward<F>(fn)](Time timeout) mutable {
                return future.wait(timeout).except([fn=etl::forward<F>(fn)](osStatus_t err) { return (fn(err), err); });
            };
        }

        /// @brief chain the result error after waiting the result
        /// @param fn callable with signature `Result<T, osStatus_t>(osStatus_t)`
        /// @return new future object with result R
        template <typename F, typename R = decltype(etl::declval<F>()(etl::declval<osStatus_t>())),
            typename = etl::enable_if_t<etl::is_etl_result_v<R>>
        > Future<T> or_except(F&& fn) && {
            return [future=etl::move(*this), fn=etl::forward<F>(fn)](Time timeout) mutable {
                return future.wait(timeout).or_except(etl::forward<F>(fn));
            };
        }

    private:
        /// alternative function object when the object creation is inside ISR context
        etl::Function<Result<T, osStatus_t>(Time), void*> static_fn; 
        std::function<Result<T, osStatus_t>(Time)> fn;
    };

    template <>
    class Future<void> {
    public:
        /// empty constructor
        Future() {}

        /// construct future function statically
        /// @param static_fn etl::Function with signature `Result<void, osStatus_t>(Time)`
        template <typename Ctx>
        Future(Function<Result<void, osStatus_t>(Time), Ctx*>&& static_fn) : static_fn(etl::move(static_fn)) {}

        /// construct future function dynamically
        /// @param fn callable with signature `Result<void, osStatus_t>(Time)`
        template <typename F, typename = etl::enable_if_t<
            etl::is_same_v<etl::Result<void, osStatus_t>, decltype(etl::declval<F>()(etl::declval<Time>()))>
        >> Future(F&& fn) : fn(etl::forward<F>(fn)) {}

        template <typename T>
        Future(Future<T>&& other) : fn([future=etl::move(other)](Time timeout) mutable -> Result<void, osStatus_t> {
            return future.wait(timeout);
        }) {}

        /// check if this future object is valid
        bool valid() const {
            return fn || static_fn;
        }

        /// wait the result indefinitely
        Result<void, osStatus_t> await() { 
            return wait(time::infinite);
        }

        /// wait the result until timeout
        Result<void, osStatus_t> wait(Time timeout) {
            if (!valid())
                return Err(osError);
            
            auto res = fn ? fn(timeout) : static_fn(timeout);
            fn = nullptr;
            static_fn = nullptr;
            return res;
        }

        /// @brief execute fn before waiting the result
        /// @param fn callable with signature `void()`
        /// @return new future object with result void
        template <typename F>
        Future<void> setup(F&& fn) && {
            return [future=etl::move(*this), fn=etl::forward<F>(fn)](Time timeout) mutable {
                fn();
                return future.wait(timeout);
            };
        }

        /// @brief execute fn after waiting the result
        /// @param fn callable with signature `void()`
        /// @return new future object with result void
        template <typename F>
        Future<void> finally(F&& fn) && {
            return [future=etl::move(*this), fn=etl::forward<F>(fn)](Time timeout) mutable {
                return future.wait(timeout).finally(etl::forward<F>(fn));
            };
        }

        /// @brief chain the result value after waiting the result
        /// @param fn callable with signature `R(void)`
        /// @return new future object with result R
        template <typename F, typename R = decltype(etl::declval<F>()())>
        Future<R> then(F&& fn) && {
            return [future=etl::move(*this), fn=etl::forward<F>(fn)](Time timeout) mutable {
                return future.wait(timeout).then(etl::forward<F>(fn));
            };
        }

        /// @brief chain the result value after waiting the result
        /// @param fn callable with signature `Result<R, osStatus_t>(void)`
        /// @return new future object with result R
        template <typename F, typename R = decltype(etl::declval<F>()()), 
            typename = etl::enable_if_t<etl::is_etl_result_v<R>>
        > Future<etl::result_value_t<R>> and_then(F&& fn) && {
            return [future=etl::move(*this), fn=etl::forward<F>(fn)](Time timeout) mutable {
                return future.wait(timeout).and_then(etl::forward<F>(fn));
            };
        }

        /// @brief chain the result value after waiting the result
        /// @param fn callable with signature `Result<R, osStatus_t>(Time)`
        /// @return new future object with result R
        template <typename F, typename R = decltype(etl::declval<F>()(etl::declval<Time>())), 
            typename = etl::enable_if_t<etl::is_etl_result_v<R>>
        > Future<etl::result_value_t<R>> and_async(F&& fn) && {
            return [future=etl::move(*this), fn=etl::forward<F>(fn)](Time timeout) mutable {
                return future.wait(timeout).and_then([fn=etl::forward<F>(fn), timeout]() { return fn(timeout); });
            };
        }

        /// @brief chain the result error after waiting the result
        /// @param fn callable with signature `void(osStatus_t)`
        /// @return new future object with result R
        template <typename F, typename = decltype(etl::declval<F>()(etl::declval<osStatus_t>()))>
        Future<void> except(F&& fn) && {
            return [future=etl::move(*this), fn=etl::forward<F>(fn)](Time timeout) mutable {
                return future.wait(timeout).except([fn=etl::forward<F>(fn)](osStatus_t err) { return (fn(err), err); });
            };
        }

        /// @brief chain the result error after waiting the result
        /// @param fn callable with signature `Result<void, osStatus_t>(osStatus_t)`
        /// @return new future object with result R
        template <typename F, typename R = decltype(etl::declval<F>()(etl::declval<osStatus_t>())),
            typename = etl::enable_if_t<etl::is_etl_result_v<R>>
        > Future<void> or_except(F&& fn) && {
            return [future=etl::move(*this), fn=etl::forward<F>(fn)](Time timeout) mutable {
                return future.wait(timeout).or_except(etl::forward<F>(fn));
            };
        }

    private:
        etl::Function<Result<void, osStatus_t>(Time), void*> static_fn;
        std::function<Result<void, osStatus_t>(Time)> fn;
    };

    template <typename T>
    class Promise {
    public:
        Promise() {}

        /// set the promised value
        template <typename U>
        Result<void, osStatus_t> set(U&& value) { 
            if (!valid()) 
                return Err(osError); // busy
            
            this->value = etl::forward<U>(value);
            osThreadFlagsSet(thread, 0b1);
            return Ok();
        }

        bool valid() const { 
            return thread; 
        }

        Result<T, osStatus_t> wait(Time timeout) {
            if (valid())
                return Err(osError); // busy
            
            thread = osThreadGetId();
            uint32_t flags = osThreadFlagsWait(0b1, osFlagsWaitAny, timeout.tick);
            thread = nullptr;

            auto status = detail::flagsToStatus(flags);
            if (status.is_err())
                return Err(status.unwrap_err());

            T res = etl::move(*value);
            value = none;
            return Ok(etl::move(res));
        }

        Result<T, osStatus_t> await() {
            return wait(time::infinite);
        }

        Future<T> get_future() {
            return etl::bind<&Promise::wait>(this);
        }

    private:
        Optional<T> value;
        osThreadId_t thread = nullptr;
    };

    template <>
    class Promise<void> {
    public:
        Promise() {}

        /// set the promised value
        Result<void, osStatus_t> set() { 
            if (!valid()) 
                return Err(osError); // busy
            
            osThreadFlagsSet(thread, 0b1);
            return Ok();
        }

        bool valid() const { 
            return thread; 
        }

        Result<void, osStatus_t> wait(Time timeout) {
            if (valid())
                return Err(osError); // busy
            
            thread = osThreadGetId();
            uint32_t flags = osThreadFlagsWait(0b1, osFlagsWaitAny, timeout.tick);
            thread = nullptr;

            auto status = detail::flagsToStatus(flags);
            if (status.is_err())
                return Err(status.unwrap_err());

            return Ok();
        }

        Result<void, osStatus_t> await() {
            return wait(time::infinite);
        }

        Future<void> get_future() {
            return etl::bind<&Promise::wait>(this);
        }

    private:
        osThreadId_t thread = nullptr;
    };
}

#endif
