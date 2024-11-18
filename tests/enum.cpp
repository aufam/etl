// #include "etl/enum.h"
#include "gtest/gtest.h"
#include "etl/keywords.h"

using namespace etl;

template <typename T>
class Some {
public:
    T value;

    template <typename U, typename = etl::enable_if_t<etl::is_convertible_v<U, T>>>
    Some(Some<U>&& v) : value(etl::move(v)) {}

    bool operator==(Some o) const { return value == o.value; }
};

template <typename T>
class EnumItem {
public:
    T value;
    constexpr EnumItem(T&& value) : value(std::move(value)) {}

    constexpr bool operator==(const EnumItem<T>& other) const { return value == other.value; }
};

template <typename ...Ts>
class Enum {
public:
    std::variant<Ts...> variant;

    Enum() = delete;

    Enum(const Enum<Ts...>&) = default;
    Enum(Enum<Ts...>&&) = default;
    Enum& operator=(const Enum<Ts...>&) = default;
    Enum& operator=(Enum<Ts...>&&) = default;
    
    template <typename T> constexpr 
    Enum(T&& value) : variant(std::forward<T>(value)) {}

    template <typename T> constexpr Enum&
    operator=(T&& value) { return (variant = std::forward<T>(value), *this); }

    template <typename T> constexpr std::enable_if_t<(std::is_same_v<T, Ts> || ...), bool>
    operator==(const T& value) const { return std::holds_alternative<T>(variant) && std::get<T>(variant) == value; } 
};

template <typename T>
class Ok: public EnumItem<T> {
public:
    constexpr Ok(T&& value) : EnumItem<T>(std::move(value)) {}
};

template <typename T>
class Err: public EnumItem<T> {
public:
    constexpr Err(T&& value) : EnumItem<T>(std::move(value)) {}
};

template <typename T, typename E>
class Result: public Enum<Ok<T>, Err<E>> {
public:
    using Enum<Ok<T>, Err<E>>::Enum;
};

TEST(Enum, Enum) {
    Result<int, const char*> a = Ok(10);

    a == Ok(10);
}