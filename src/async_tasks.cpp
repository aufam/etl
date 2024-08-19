#include "etl/async_task.h"
#include "etl/thread.h"
#include "etl/bit.h"

using namespace Project::etl;

static Thread<ETL_ASYNC_TASK_THREAD_SIZE> threads[ETL_ASYNC_N_CHANNELS];
task::detail::Sender task::detail::senders[ETL_ASYNC_N_CHANNELS];
task::detail::Receiver task::detail::receivers[ETL_ASYNC_N_CHANNELS];

Future<void> task::sleep(Time timeout) {
    return [timeout] (Time) -> Result<void, osStatus_t> {
        auto res = osDelay(timeout.tick);
        if (res == osOK) return Ok();
        else return Err(res);
    };
}

void task::init() {
    int prio = osPriorityAboveNormal;
    for (auto& thd: threads)
        thd.init({.function=&detail::execute, .prio=(osPriority_t) prio++});
}

int task::resources() {
    int cnt = 0;
    for (auto &sender: detail::senders) if (!sender.promise) {
        ++cnt;
    }

    return cnt;
}

void task::terminate() {
    for (int i = 0; i < ETL_ASYNC_N_CHANNELS; ++i)
        etl::ignore = threads[i].setFlags(1u << ETL_ASYNC_N_CHANNELS);
}

void task::detail::execute() {
    while (true) {
        auto flag = osThreadFlagsWait(0xFFFFFF, osFlagsWaitAny, osWaitForever);
        if (flag & osFlagsError)
            break;

        auto channel = etl::count_trailing_zeros(flag);
        if (channel >= ETL_ASYNC_N_CHANNELS)
            break;

        auto sender = &senders[channel];
        auto receiver = &receivers[channel];

        auto result = sender->promise();
        sender->closure();

        if (receiver->promise.valid())
            receiver->promise.set(result);
        else
            ::free(result);

        ::memset(sender->mempool, 0, sizeof(sender->mempool));
        sender->promise = nullptr;
        sender->closure = nullptr;
    }

    etl::this_thread::exit();
}

int task::detail::select_channel() {
    int i = 0;
    for (; i < ETL_ASYNC_N_CHANNELS; ++i) {
        if (!senders[i].promise)
            break;
    }

    return i;
}

void task::detail::trigger_thread(int channel) {
    etl::ignore = threads[channel].setFlags(1 << channel);
}
