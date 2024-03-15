#include "etl/async_task.h"
#include "etl/bit.h"

namespace Project::etl {
    Tasks* Tasks::self = nullptr;

    void Tasks::init() {
        self = this;
        
        int prio = osPriorityAboveNormal;
        for (auto& thd: threads)
            thd.init({.function=etl::bind<&Tasks::execute>(this), .prio=(osPriority_t) prio++});

        for (auto& receiver: receivers)
            receiver.result.init();
    }

    void Tasks::execute() {
        while (true) {
            auto flag = etl::this_thread::waitFlagsAny();
            if (not flag)
                break;

            auto channel = etl::count_trailing_zeros(flag.get());            
            auto sender = &senders[channel];
            auto receiver = &receivers[channel];
            
            auto result = sender->promise();
            sender->closure();

            if (receiver->is_waiting)
                receiver->result.push(result);
            
            receiver->is_waiting = false;
            ::memset(sender->mempool, 0, sizeof(sender->mempool));
            sender->promise = nullptr;
            sender->closure = nullptr;
        }

        etl::this_thread::exit();
    }

    int Tasks::resources() {
        int cnt = 0;
        for (auto &sender: senders) if (!sender.promise) {
            ++cnt;
        }

        return cnt;
    }

    void Tasks::terminate() {
        for (int i = 0; i < ETL_ASYNC_N_CHANNELS; ++i) {
            threads[i].setFlags(osFlagsError);
        }
    }

    int Tasks::select_channel() {
        int i = 0;
        for (; i < ETL_ASYNC_N_CHANNELS; ++i) {
            if (!senders[i].promise && receivers[i].result.len() == 0)
                break;
        }

        return i;
    }
}