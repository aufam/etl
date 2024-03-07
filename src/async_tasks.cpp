#include "etl/async_task.h"

namespace Project::etl {
    Tasks tasks;

    int Tasks::resources() {
        int cnt = 0;
        for (auto &it : tasks) if (it.is_running && !it.busy) ++cnt;
        return cnt;
    }

    void Tasks::terminate_all() {
        for (auto &task: tasks) {
            task.thread.setFlags(0b10);
            task.is_running = false;
        }
    }

    Task* Tasks::select_task() {
        int prio = osPriorityHigh;
        for (auto &task : tasks) task.init((osPriority_t) prio--, "");
        if (!tasks[0].thread.get())
            return nullptr;

        Task* task = nullptr;
        for (auto &it : tasks) if (it.is_running && !it.busy) {
            task = &it;
            break;
        }

        return task;
    }

    void Task::init(osPriority_t prio, const char* name) {
        thread.init({.function=etl::bind<&Task::execute>(this), .prio=prio, .name=name, });
        que.init({.name=name, });
    }

    void Task::execute() {
        while (is_running) {
            auto flag = etl::this_thread::waitFlagsAny();
            if (flag & 0b10) {
                break;
            }

            auto ptr = que.pop();
            ::free(ptr); 
            que.push(is_static ? fn_static() : fn());
            cleanup();
        }

        etl::this_thread::exit();
    }

    void Task::cleanup() {
        if (is_static)
            free_args();
        
        fn = nullptr;
        fn_static = nullptr;
        busy = false;
        is_static = false;
    }
}