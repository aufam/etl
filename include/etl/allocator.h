#ifndef ETL_ALLOCATOR_H
#define ETL_ALLOCATOR_H

#include "etl/utility_basic.h"
#include <cstdlib>

namespace Project::etl {
    template <typename T>
    class Allocator {
        static_assert(!is_void_v<T> && !is_const_v<T> && !is_volatile_v<T>);

    public:
        constexpr Allocator() {}
        
        T* allocate(size_t n) { return (T*) ::malloc(n * sizeof(T)); }
        void deallocate(T* p, size_t) { ::free((void*)p); }
    };
}

#endif