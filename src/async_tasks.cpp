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
            auto flag = osThreadFlagsWait(0xFFFFFF, osFlagsWaitAny, osWaitForever);
            if (flag & osFlagsError) {
                break;
            }

            auto channel = etl::count_trailing_zeros(flag);
            if (channel >= ETL_ASYNC_N_CHANNELS) {
                break;
            }

            auto sender = &senders[channel];
            auto receiver = &receivers[channel];

            auto result = sender->promise();
            sender->closure();

            if (receiver->is_waiting)
                receiver->result.push(result).await();
            
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
        for (int i = 0; i < ETL_ASYNC_N_CHANNELS; ++i)
            etl::ignore = threads[i].setFlags(1u << ETL_ASYNC_N_CHANNELS);
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