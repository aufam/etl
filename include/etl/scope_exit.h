#ifndef ETL_SCOPE_EXIT_H
#define ETL_SCOPE_EXIT_H

#include "etl/utility_basic.h"

namespace Project::etl {

    /// manages cleanup actions on scope exit using RAII principles.
    template <typename Callable>
    class ScopeExit {
        Callable onExit;
    
    public:
        ScopeExit(Callable&& onExit) : onExit(etl::forward<Callable>(onExit)) {}
        ~ScopeExit() { onExit(); }

        ScopeExit(const ScopeExit&) = delete;             // No copy constructor
        ScopeExit& operator=(const ScopeExit&) = delete;  // No copy assignment
        ScopeExit(ScopeExit&&) = default;                 // Default move constructor
        ScopeExit& operator=(ScopeExit&&) = default;      // Default move assignment
    };
}

#endif //ETL_SCOPE_EXIT_H
