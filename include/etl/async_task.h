#ifndef ETL_ASYNC_TASK_H
#define ETL_ASYNC_TASK_H

#include "etl/thread.h"
#include "etl/queue.h"
#include "etl/memory.h"
#include "etl/function.h"
#include "etl/async.h"

#ifndef ETL_ASYNC_TASKS_SIZE
#define ETL_ASYNC_TASKS_SIZE 4
#endif

#ifndef ETL_ASYNC_TASK_THREAD_SIZE
#define ETL_ASYNC_TASK_THREAD_SIZE 1024
#endif

namespace Project::etl {
    extern Tasks tasks;

    template <typename F, typename... Args>
    auto async(F&& fn, Args&&... args);

    class Task {
        friend class Tasks;
        Task() = default;

        void init(osPriority_t prio, const char* name);
        void execute();
        void cleanup();

        etl::Thread<ETL_ASYNC_TASK_THREAD_SIZE> thread;
        etl::Queue<void*, 1> que;
        uint8_t args[32] = {};

        etl::Function<void*(), void*> fn_static;
        etl::Function<void(), void*> free_args;
        std::function<void*()> fn;

        bool is_running = true;
        bool busy = false;
        bool is_static = false;
    };


    class Tasks {
    public:
        struct LaunchArgs {
            bool ignore_result = false;
            etl::Time timeout = etl::time::infinite;
            std::function<void()> abort_fn = nullptr;
        };

        int resources();
        void terminate_all();

        template <typename T> Future<etl::conditional_t<etl::is_void_v<T>, void, etl::unique_ptr<T>>>
        launch(Future<T>&& future, LaunchArgs args);

        template <typename T>
        auto launch(Future<T>&& future) { return launch(etl::move(future), LaunchArgs{.timeout=etl::time::infinite}); }

        template <typename T>
        void launch(const Future<T>&, LaunchArgs) = delete;

    private:
        Task* select_task();

        Task tasks[ETL_ASYNC_TASKS_SIZE];

        template <typename F, typename... Args>
        friend auto async(F&& fn, Args&&... args);

        template <auto method, typename Class, typename... Args>
        friend auto async(Class* self, Args&&... args);

        template <typename TAsync, typename... Args>
        friend auto async(const etl::Async<TAsync>* fn, Args&&... args);

        template <typename F, typename... Args>
        auto launch_static(F&& fn, Args&&... args);
    };

    template <typename F, typename... Args>
    auto async(F&& fn, Args&&... args) {
        return tasks.launch_static(etl::forward<F>(fn), etl::forward<Args>(args)...);
    }

    template <auto method, typename Class, typename... Args>
    auto async(Class* self, Args&&... args) {
        return tasks.launch_static(etl::bind<method>(self), etl::forward<Args>(args)...);
    }

    template <typename TAsync, typename... Args>
    auto async(const etl::Async<TAsync>* fn, Args&&... args) {
        return tasks.launch_static(+[](const etl::Async<TAsync>* fn, Args&&... args) {
            return (*fn)(etl::forward<Args>(args)...).await();
        }, fn, etl::forward<Args>(args)...);
    }

    template <typename TAsync, typename... Args>
    auto async(const etl::Async<TAsync>& fn, Args&&... args) {
        return async(&fn, etl::forward<Args>(args)...);
    }

    template <typename T> Future<etl::conditional_t<etl::is_void_v<T>, void, etl::unique_ptr<T>>>
    Tasks::launch(Future<T>&& future, LaunchArgs args) {
        auto task = select_task();
        if (task != nullptr) {
            if constexpr (etl::is_void_v<T>) {
                task->fn = [fn=etl::move(future.fn)]() -> void* { return (fn(), nullptr); };
            } else {
                task->fn = [fn=etl::move(future.fn), ignore=args.ignore_result]() -> void* { 
                    return ignore ? (fn(), nullptr) : new T(fn()); 
                };
            }
            task->busy = true;
            task->thread.setFlags(0b1);
        }

        return [task, timeout=args.timeout, abort_fn=etl::move(args.abort_fn)] { 
            void* res = nullptr;
            if (task != nullptr && task->que.pop(res, timeout) < 0 && abort_fn) {
                abort_fn();
            }

            if constexpr (!etl::is_void_v<T>) {
                return etl::unique_ptr<T>(static_cast<T*>(res));
            }
        };
    }

    template <typename F, typename... Args>
    auto Tasks::launch_static(F&& fn, Args&&... args) {
        using Fn = etl::conditional_t<etl::is_function_reference_v<F>, etl::add_pointer_t<etl::remove_reference_t<F>>, F>;
        using ResultType = decltype(etl::declval<F>()(etl::declval<Args>()...));
        using Result = etl::conditional_t<etl::is_void_v<ResultType>, void, etl::unique_ptr<ResultType>>;
        using FnArgs = std::pair<Fn, std::tuple<Args...>>;

        auto task = select_task();
        if (task == nullptr)
            return Future<Result>(etl::Function<Result(), void*>());

        auto fn_args = new (task->args) FnArgs(std::make_pair(
            etl::forward<Fn>(fn), 
            std::forward_as_tuple(etl::forward<Args>(args)...)
        ));

        task->fn_static = {[] (FnArgs* fn_args) -> void* {
            auto& fn = fn_args->first;
            auto& args = fn_args->second;

            if constexpr (etl::is_void_v<ResultType>) {
                std::apply(etl::move(fn), etl::move(args));
                return nullptr;
            } else {
                return new ResultType(std::apply(etl::move(fn), etl::move(args)));
            }
        }, fn_args};

        task->free_args = {[] (FnArgs* fn_args) {
            fn_args->~FnArgs();
        }, fn_args};

        task->busy = true;
        task->is_static = true;
        task->thread.setFlags(0b1);

        return Future<Result>(etl::Function<Result(), void*>([] (Task* task) { 
            if (task == nullptr)
                return Result();
            
            void* res = nullptr;
            task->que.pop(res);
            if constexpr (!etl::is_void_v<ResultType>) {
                return Result(static_cast<ResultType*>(res));
            }
        }, task));
    }
}

#endif