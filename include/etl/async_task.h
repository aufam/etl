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
#define ETL_ASYNC_TASK_THREAD_SIZE 512
#endif

namespace Project::etl {
    extern Tasks tasks;

    class Task {
        friend class Tasks;

        void init(osPriority_t prio, const char* name) {
            thread.init({.function=etl::bind<&Task::execute>(this), .prio=prio, .name=name, });
            que.init({.name=name, });
        }

        void execute() {
            for (;;) {
                etl::this_thread::waitFlagsAny();
                auto ptr = que.pop();
                ::free(ptr); 
                que.push(from_isr ? (fn_from_isr(), nullptr) : fn());
                cleanup();
            }
        }

        void cleanup() {
            fn = nullptr;
            fn_from_isr = nullptr;
            busy = false;
            from_isr = false;
        }

        etl::Thread<ETL_ASYNC_TASK_THREAD_SIZE> thread;
        etl::Queue<void*, 1> que;

        std::function<void*()> fn;
        etl::Function<void(), void*> fn_from_isr;
        bool busy = false;
        bool from_isr = false;
    };


    class Tasks {
    public:
        struct LaunchArgs {
            bool ignore_result = false;
            etl::Time timeout = etl::time::infinite;
            std::function<void()> abort_fn = nullptr;
        };

        int resources() {
            int cnt = 0;
            for (auto &it : tasks) if (not it.busy) ++cnt;
            return cnt;
        }

        void launch_from_isr(etl::Function<void(), void*> fn) {
            auto task = select_task();
            if (task != nullptr) {
                task->busy = true;
                task->from_isr = true;
                task->fn_from_isr = fn;
                task->thread.setFlags(0b1);
            }
        }

        template <typename R, typename... Args, typename... InnerArgs>
        void launch_from_isr(const etl::Async<R(Args...)>& async, InnerArgs... args) {
            async.assign_args(std::forward<InnerArgs>(args)...);
            launch_from_isr({[] (const etl::Async<R(Args...)>* async) { async->invoke(); }, &async});
        }

        template <typename T>
        Future<etl::unique_ptr<T>> launch(Future<T>&& future, LaunchArgs args) {
            auto task = select_task();
            if (task != nullptr) {
                task->busy = true;
                task->fn = [fn=etl::move(future.fn), ignore=args.ignore_result] { return ignore ? (fn(), nullptr) : new T(fn()); };
                task->thread.setFlags(0b1);
            }
            
            return [que=task ? &task->que : nullptr, timeout=args.timeout, abort_fn=etl::move(args.abort_fn)] { 
                if (not que)
                    return etl::unique_ptr<T>();
                
                void* res = nullptr;
                if (que->pop(res, timeout) < 0 and abort_fn)
                    abort_fn();
                
                return etl::unique_ptr<T>(reinterpret_cast<T*>(res));
            };
        }

        Future<void> launch(Future<void>&& future, LaunchArgs args) {
            auto task = select_task();
            if (task != nullptr) {
                task->busy = true;
                task->fn = [fn=etl::move(future.fn)] { return (fn(), nullptr); };
                task->thread.setFlags(0b1);
            }
            
            return [que=task ? &task->que : nullptr, timeout=args.timeout, abort_fn=etl::move(args.abort_fn)] { 
                if (not que)
                    return;

                void* res;
                if (que->pop(res, timeout) < 0 and abort_fn)
                    abort_fn();
            };
        }

        template <typename T>
        auto launch(Future<T>&& future) { return launch(etl::move(future), LaunchArgs{.timeout=etl::time::infinite}); }

        template <typename R, typename... Args>
        void launch(Future<R(Args...)>, LaunchArgs) = delete;

    private:
        Task* select_task() {
            int prio = osPriorityHigh;
            for (auto &task : tasks) task.init((osPriority_t) prio--, "");
            if (not tasks[0].thread.get())
                return nullptr;

            Task* task = nullptr;
            for (auto &it : tasks) if (not it.busy) {
                task = &it;
                break;
            }

            return task;
        }

        Task tasks[ETL_ASYNC_TASKS_SIZE];
    };
}

#endif