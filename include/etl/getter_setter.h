#ifndef ETL_GETTER_SETTER_H
#define ETL_GETTER_SETTER_H

#include "etl/utility_basic.h"

namespace Project::etl {

    template <typename T, typename GetterFunction>
    struct Getter {
        GetterFunction get;

        constexpr operator T() const { return get(); }

        template <typename U>
        constexpr decltype(auto) operator==(U&& value) const { return get() == etl::forward<U>(value); }

        template <typename U>
        void operator=(U&&) = delete;
    };
    

    template <typename T, typename SetterFunction>
    struct Setter {
        SetterFunction set;

        template <typename U>
        constexpr decltype(auto) operator=(U&& value) const { return set(etl::forward<U>(value)); }

        template <typename U>
        bool operator==(U&& value) = delete;

        operator T() = delete;
    };


    template <typename T, typename GetterFunction, typename SetterFunction>
    struct GetterSetter {
        GetterFunction get;
        SetterFunction set;

        constexpr operator T() const { return get(); }

        template <typename U>
        constexpr decltype(auto) operator==(U&& value) const { return get() == etl::forward<U>(value); }

        template <typename U>
        constexpr decltype(auto) operator=(U&& value) const { return set(etl::forward<U>(value)); }
    };


    template <typename T, typename GetterFunction> constexpr auto
    getter(GetterFunction&& get) { return Getter<T, GetterFunction> { etl::forward<GetterFunction>(get) }; }

    template <typename T, typename SetterFunction> constexpr auto
    setter(SetterFunction&& set) { return Setter<T, SetterFunction> { etl::forward<SetterFunction>(set) }; }

    template <typename T, typename GetterFunction, typename SetterFunction> constexpr auto
    getter_setter(GetterFunction&& get, SetterFunction&& set) { 
        return GetterSetter<T, GetterFunction, SetterFunction> { 
            etl::forward<GetterFunction>(get), 
            etl::forward<SetterFunction>(set) 
        }; 
    }

}

#endif