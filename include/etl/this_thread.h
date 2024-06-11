#ifndef ETL_THIS_THREAD_H
#define ETL_THIS_THREAD_H

#include "etl/event.h"

namespace Project::etl::this_thread {

    using time::sleep;
    using time::sleepUntil;

    /// return the current running thread object
    /// @note should be called in thread function
    inline auto get() { return osThreadGetId(); }

    /// pass control from current thread to next thread
    /// @return osStatus
    /// @note should be called in thread function
    inline osStatus_t yield() { return osThreadYield(); }

    /// terminate execution of current running thread
    /// @note should be called in thread function
    inline void exit() { osThreadExit(); }

    /// reset the specified flags of current running thread
    /// @param flags specifies the flags of the thread that shall be reset
    /// @return current thread's flags before resetting or error code if highest bit set
    /// @note should be called in thread function
    inline Result<uint32_t, osStatus_t> resetFlags(uint32_t flags) { return detail::eventResult(osThreadFlagsClear(flags)); }

    /// get the current flags of the current running thread
    /// @return current thread's flags
    /// @note should be called in thread function
    inline Result<uint32_t, osStatus_t> getFlags() { return detail::eventResult(osThreadFlagsGet()); }

    /// @brief Fetches specific flags of the current running thread.
    /// @param args Arguments for fetching flags:
    ///             - .flags specifies the flags to fetch
    ///             - .option osFlagsWaitAny (default) or osFlagsWaitAll
    ///             - .doReset specifies whether to reset the flags or not, default = true
    /// @return A Future object representing the flags before resetting.
    ///         The caller needs to invoke .wait(timeout) or await() to get the result object of the flags.
    /// @note This function should be called in the thread function.
    inline Future<uint32_t> fetchFlags(Event::WaitFlagsArgs args) { 
        return [args] (Time timeout) mutable -> Result<uint32_t, osStatus_t> {
            if (!args.doReset) args.option |= osFlagsNoClear;
            return detail::eventResult(osThreadFlagsWait(args.flags, args.option, timeout.tick)); 
        };
    }

    /// @brief Fetches any flags of the current running thread.
    /// @param args Arguments for fetching flags:
    ///             - .doReset specifies whether to reset the flags or not, default = true
    /// @return A Future object representing the flags before resetting.
    ///         The caller needs to invoke .wait(timeout) or await() to get the result object of the flags.
    /// @note This function should be called in the thread function.
    inline Future<uint32_t> fetchAnyFlags(Event::WaitFlagsAnyArgs args = {}) { 
        return [args] (Time timeout) mutable -> Result<uint32_t, osStatus_t> {
            const uint32_t flags = (1u << 24) - 1; // all possible flags
            uint32_t option = osFlagsWaitAny;
            if (!args.doReset) option |= osFlagsNoClear;
            return detail::eventResult(osThreadFlagsWait(flags, option, timeout.tick)); 
        };
    }
}

#endif //ETL_THIS_THREAD_H
