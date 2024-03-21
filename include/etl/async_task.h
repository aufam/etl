#ifndef ETL_ASYNC_TASK_H
#define ETL_ASYNC_TASK_H

#include "etl/thread.h"
#include "etl/queue.h"
#include "etl/memory.h"
#include "etl/function.h"
#include "etl/future.h"

#ifndef ETL_ASYNC_N_CHANNELS
#define ETL_ASYNC_N_CHANNELS 4
#endif

#ifndef ETL_ASYNC_TASK_THREAD_SIZE
#define ETL_ASYNC_TASK_THREAD_SIZE 256
#endif

namespace Project::etl {
    template <typename F, typename... Args>
    auto async(F&& fn, Args&&... args);

    class Tasks {
    public:
        struct LaunchArgs {
            bool ignore_result = false;
            etl::Time timeout = etl::time::infinite;
            std::function<void()> abort_fn = nullptr;
        };

        void init();
        int resources();
        void terminate();

    private:
        static Tasks *self;

        struct Sender {
            uint8_t mempool[32];
            etl::Function<void*(), void*> promise;
            etl::Function<void(), void*> closure;
        };

        struct Receiver {
            etl::Queue<void*, 1> result;
            bool is_waiting = false;
        };

        int select_channel();
        void execute();

        etl::Thread<ETL_ASYNC_TASK_THREAD_SIZE> threads[ETL_ASYNC_N_CHANNELS];
        Sender senders[ETL_ASYNC_N_CHANNELS];
        Receiver receivers[ETL_ASYNC_N_CHANNELS];

        template <typename F, typename... Args>
        friend auto async(F&& fn, Args&&... args);

        template <auto method, typename Class, typename... Args>
        friend auto async(Class* self, Args&&... args);

        template <typename F, typename... Args>
        auto launch(F&& fn, Args&&... args);
    };

    template <typename F, typename... Args>
    auto Tasks::launch(F&& fn, Args&&... args) {
        using Fn = etl::conditional_t<etl::is_function_reference_v<F>, etl::add_pointer_t<etl::remove_reference_t<F>>, F>;
        using T = decltype(etl::declval<F>()(etl::declval<Args>()...));
        using FnArgs = std::pair<Fn, std::tuple<Args...>>;
        
        auto channel = select_channel(); 
        if (channel >= ETL_ASYNC_N_CHANNELS)
            return Future<T>();

        auto sender = &senders[channel];
        auto receiver = &receivers[channel];

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

        etl::ignore = threads[channel].setFlags(1 << channel);

        return Future<T>(Function<Result<T, osStatus_t>(Time), void*>([] (Receiver* receiver, Time timeout) -> Result<T, osStatus_t> { 
            if (receiver == nullptr)
                return Err(osError);

            receiver->is_waiting = true;
            auto [res, err] = receiver->result.pop().wait(timeout);
            receiver->is_waiting = false;

            if (err)
                return Err(*err);
            
            if constexpr (!etl::is_void_v<T>) {
                T* ptr = reinterpret_cast<T*>(*res);
                T value = etl::move(*ptr);
                delete ptr;

                return Ok(etl::move(value));
            } else {
                return Ok();
            }

        }, receiver));
    }
}

namespace Project::etl::task {
    inline Future<void> sleep(Time timeout) {
        return [timeout] (Time) -> Result<void, osStatus_t> {
            auto res = osDelay(timeout.tick);
            if (res == osOK) return Ok();
            else return Err(res);
        };
    }
}

#endif