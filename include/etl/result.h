#ifndef ETL_RESULT_H
#define ETL_RESULT_H

#include "etl/utility_basic.h"
#include "cmsis_os2.h"

namespace Project::etl {

	template <typename T>
    class Result {
    public:
        using type = T;

        Result() : status(osError) {}

        Result(osStatus_t status) : status(status == osOK ? osError : status) {}

        Result(const T& value) : status(osOK) {
            new (buffer) T(value);
        }

        Result(T&& value) : status(osOK) {
            new (buffer) T(etl::move(value));
        }

        Result(const Result&) = delete;
        Result(Result&&) = delete;

        Result& operator=(Result&& other) {
			if (this == &other)
				return *this;

            this->~Result();
            status = etl::exchange(other.status, osError);
            etl::copy(other.buffer, buffer);
            return *this;
        }

        operator T() const {
            if (status == osOK) status = osError; 
            return etl::move(*reinterpret_cast<T*>(buffer)); 
        }

        T* operator->() {
            return reinterpret_cast<T*>(buffer);
        }

        const T* operator->() const {
            return reinterpret_cast<const T*>(buffer);
        }

        bool operator==(osStatus_t other) const { 
            return other == status; 
        }

        bool operator!=(osStatus_t other) const { 
            return other != status; 
        }
        
        ~Result() {
            if (status == osOK) {
                reinterpret_cast<T*>(buffer)->~T();
                status = osError;
            }
        }

    private:
        mutable uint8_t buffer[sizeof(T)] = {};
    
    public:
        mutable osStatus_t status;
    };

    template <>
    class Result<void> {
    public:
        using type = void;

        Result() : status(osError) {}

        Result(osStatus_t status) : status(status) {}

        Result(const Result&) = delete;
        Result(Result&&) = delete;

        Result& operator=(Result&& other) {
            status = etl::exchange(other.status, osError);
            return *this;
        }

        bool operator==(osStatus_t other) const { 
            return other == status; 
        }

        bool operator!=(osStatus_t other) const { 
            return other == status; 
        }

        osStatus_t status;
    };
}

#endif
