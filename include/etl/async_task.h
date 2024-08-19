#ifndef ETL_ASYNC_TASK_H
#define ETL_ASYNC_TASK_H

#include "etl/future.h"

#ifndef ETL_ASYNC_N_CHANNELS
#define ETL_ASYNC_N_CHANNELS 4
#endif

#ifndef ETL_ASYNC_TASK_THREAD_SIZE
#define ETL_ASYNC_TASK_THREAD_SIZE 256
#endif

namespace Project::etl::task {
    Future<void> sleep(Time timeout);
    void init();
    int resources();
    void terminate();
}

namespace Project::etl::task::detail {
    struct Sender {
        uint8_t mempool[32];
        etl::Function<void*(), void*> promise;
        etl::Function<void(), void*> closure;
    };

    struct Receiver {
        Promise<void*> promise;

        template <typename T>
        Result<T, osStatus_t> wait(Time timeout) {
            auto res = promise.wait(timeout);
            if (res.is_err())
                return Err(res.unwrap_err());
            
            if constexpr (!etl::is_void_v<T>) {
                T* ptr = reinterpret_cast<T*>(res.unwrap());
                T value = etl::move(*ptr);
                delete ptr;

                return Ok(etl::move(value));
            } else {
                return Ok();
            }
        }

        template <typename T>
        Future<T> get_future() { return etl::bind<&Receiver::wait<T>>(this); }
    };

    int select_channel();
    void execute();
    void trigger_thread(int channel);

    extern Sender senders[ETL_ASYNC_N_CHANNELS];
    extern Receiver receivers[ETL_ASYNC_N_CHANNELS];
}

namespace Project::etl::task {
    template <typename F, typename... Args>
    auto launch(F&& fn, Args&&... args) {
        using Fn = etl::conditional_t<etl::is_function_reference_v<F>, etl::add_pointer_t<etl::remove_reference_t<F>>, F>;
        using T = decltype(etl::declval<F>()(etl::declval<Args>()...));
        using FnArgs = std::pair<Fn, std::tuple<Args...>>;
        
        auto channel = detail::select_channel(); 
        if (channel >= ETL_ASYNC_N_CHANNELS)
            return Future<T>();

        auto sender = &detail::senders[channel];
        auto receiver = &detail::receivers[channel];

        auto fn_args = new (sender->mempool) FnArgs(std::make_pair(
            etl::forward<Fn>(fn), 
            std::forward_as_tuple(etl::forward<Args>(args)...)
        ));

        sender->promise = {[] (FnArgs* fn_args) -> void* {
            auto& fn = fn_args->first;
            auto& args = fn_args->second;

            if constexpr (etl::is_void_v<T>) {
                std::apply(etl::move(fn), etl::move(args));
                return nullptr;
            } else {
                return new T(std::apply(etl::move(fn), etl::move(args)));
            }
        }, fn_args};

        sender->closure = {[] (FnArgs* fn_args) {
            fn_args->~FnArgs();
        }, fn_args};

        detail::trigger_thread(channel);
        return receiver->get_future<T>();
    }
}

#endif