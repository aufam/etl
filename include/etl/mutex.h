#ifndef ETL_MUTEX_H
#define ETL_MUTEX_H

#include "etl/this_thread.h"

namespace Project::etl {

    /// @class LockGuard
    /// @brief RAII wrapper for mutex locking.
    /// 
    /// This class provides a convenient RAII (Resource Acquisition Is Initialization) wrapper
    /// for locking mutexes. It automatically acquires the mutex upon construction and releases
    /// it upon destruction, ensuring proper resource management.
    /// 
    /// @note Instances of this class are marked as nodiscard to ensure that the return value
    /// of functions returning LockGuard instances is not ignored.
    class [[nodiscard]] LockGuard {
        osMutexId_t id; ///< The ID of the mutex being guarded.
        bool lock; ///< Flag indicating whether the mutex is locked.

    public:
        /// @brief Constructs a LockGuard instance.
        /// @param id The ID of the mutex to guard.
        explicit LockGuard(osMutexId_t id) : id(id), lock(false) {}

        LockGuard(const LockGuard& other) = delete;

        /// @brief Move constructor for LockGuard.
        /// @param other The LockGuard instance to move from.
        LockGuard(LockGuard&& other) noexcept : id(etl::exchange(other.id, nullptr)), lock(etl::exchange(other.lock, false)) {}

        /// @brief Destructor for LockGuard.
        ~LockGuard() { unlock(); }

        /// @brief Waits for the mutex to become available.
        /// @param timeout The maximum time to wait for the mutex.
        /// @return A Result object containing either a LockGuard instance if the mutex was acquired successfully,
        ///         or an error code if the operation failed.
        Result<LockGuard, osStatus_t> wait(Time timeout) {
            auto status = osMutexAcquire(id, timeout.tick); 
            if (status == osOK) {
                lock = true;
                return Ok(etl::move(*this));
            } else {
                return Err(status);
            }
        }

        /// @brief Waits indefinitely for the mutex to become available.
        /// @return A Result object containing either a LockGuard instance if the mutex was acquired successfully,
        ///         or an error code if the operation failed.
        Result<LockGuard, osStatus_t> await() {
            return wait(etl::time::infinite);
        }

        void unlock() {
            if (lock) osMutexRelease(id);
            lock = false;
        }
    };

    inline void unlock(Result<LockGuard, osStatus_t>& lock) { if (lock.is_ok()) lock.unwrap().unlock(); }

    /// FreeRTOS mutex interface.
    /// @note requires cmsis os v2, USE_TRACE_FACILITY, SUPPORT_STATIC_ALLOCATION, SUPPORT_DYNAMIC_ALLOCATION
    /// @note should not be declared as const
    class MutexInterface {
        static_assert(configUSE_TRACE_FACILITY > 0, "configUSE_TRACE_FACILITY has to be activated");
        static_assert(configSUPPORT_STATIC_ALLOCATION > 0, "configSUPPORT_STATIC_ALLOCATION has to be activated");
        static_assert(configSUPPORT_DYNAMIC_ALLOCATION > 0, "configSUPPORT_DYNAMIC_ALLOCATION has to be activated");

    protected:
        osMutexId_t id; ///< mutex pointer

        void referenceCounterInc() { if (id) ++reinterpret_cast<StaticSemaphore_t*>(id)->uxDummy8; }
        void referenceCounterDec() { if (id) --reinterpret_cast<StaticSemaphore_t*>(id)->uxDummy8; }

    public:
        /// empty constructor
        constexpr MutexInterface() : id(nullptr) {}

        /// construct from mutex
        explicit MutexInterface(osMutexId_t id) : id(id) {
            referenceCounterInc();
        }

        /// copy constructor
        MutexInterface(const MutexInterface& m) : id(m.id) {
            referenceCounterInc();
        }

        /// copy assignment
        MutexInterface& operator=(const MutexInterface& m) {
            if (id == m.id) return *this;
            detach();
            id = m.id;
            referenceCounterInc();
            return *this;
        }

        /// move constructor
        MutexInterface(MutexInterface&& m) : id(etl::exchange(m.id, nullptr)) {}

        /// move assignment
        MutexInterface& operator=(MutexInterface&& m) { 
            if (id == m.id) return *this;
            detach(); 
            id = etl::exchange(m.id, nullptr); 
            return *this;
        }

        /// default destructor
        ~MutexInterface() { detach(); }
        
        /// return true if id is not null
        explicit operator bool() { return count() > 0; }

        /// get the reference counter
        uint32_t count() { return reinterpret_cast<StaticSemaphore_t*>(id)->uxDummy8; }

        osMutexId_t get() { return id; }

        /// unlock mutex, delete resource, and set id to null
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t detach() { 
            referenceCounterDec();
            if (count() > 0)
                return osOK;
            
            return osMutexDelete(etl::exchange(id, nullptr)); 
        }

        /// name as null terminated string
        const char* getName() { return osMutexGetName(id); }

        auto lock() { return LockGuard(id); }

        /// get thread that owns this mutex
        /// @return thread object
        /// @note cannot be called from ISR
        auto getOwner() { return osMutexGetOwner(id); }
    };

    struct MutexAttributes {
        const char* name = nullptr; 
    };

    /// FreeRTOS static mutex.
    /// @note requires cmsis os v2
    /// @note should not be declared as const
    class Mutex : public MutexInterface {
        StaticSemaphore_t controlBlock = {};

    public:
        /// default constructor
        constexpr Mutex() : MutexInterface() {}

        Mutex(const Mutex&) = delete; ///< disable copy constructor
        Mutex(Mutex&& t) = delete;    ///< disable move constructor

        Mutex& operator=(const Mutex&) = delete;  ///< disable copy assignment
        Mutex& operator=(Mutex&&) = delete;       ///< disable move assignment

        /// initiate mutex
        /// @param name string name, default null
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t init(MutexAttributes attributes = {}) {
            if (this->id) return osError;
            
            osMutexAttr_t attr = {};
            attr.name = attributes.name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);

            this->id = osMutexNew(&attr);
            this->referenceCounterInc();
            return osOK;
        }

        /// deinit mutex
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t deinit() { return detach(); }
    };

    /// create dynamic mutex
    inline auto mutex(MutexAttributes attributes = {}) { 
        osMutexAttr_t attr = {};
        attr.name = attributes.name;
        return MutexInterface(osMutexNew(&attr)); 
    }

    /// return reference to the static mutex
    inline auto mutex(Mutex& mtx) { return MutexInterface(mtx.get()); }

    /// return reference to mutex pointer
    inline auto mutex(osMutexId_t mtx) { return MutexInterface(mtx); }

    /// return reference to the dynamic mutex
    inline auto mutex(MutexInterface& mtx) { return MutexInterface(mtx.get()); }

    /// return reference to the moved dynamic mutex
    inline auto mutex(MutexInterface&& mtx) { return MutexInterface(etl::move(mtx)); }
}

#endif //ETL_MUTEX_H
