#ifndef ETL_TYPE_TRAITS_H
#define ETL_TYPE_TRAITS_H

#if __cplusplus < 201703L
#error "Your code must be compiled with C++17 or later."
#endif

#include <cstddef>
#include <cstdint>
#include <initializer_list>

namespace Project::etl {

    /// integral constant
    template <typename T, const T V>
    struct integral_constant {
        typedef T value_type;
        typedef integral_constant<T, V> type;
        static const T value = V;
    };
    template <typename T, const T V> const T integral_constant<T, V>::value;

    /// bool constant
    typedef integral_constant<bool, false> false_type;
    typedef integral_constant<bool, true>  true_type;
    template <bool B> using bool_constant = etl::integral_constant<bool, B>;

    template <typename T> struct always_false : etl::false_type {};

    /// is_lvalue_reference
    template <typename T> struct is_lvalue_reference     : etl::false_type {};
    template <typename T> struct is_lvalue_reference<T&> : etl::true_type  {};
    template <typename T> inline constexpr bool is_lvalue_reference_v = etl::is_lvalue_reference<T>::value;

    /// is_rvalue_reference
    template <typename T> struct is_rvalue_reference      : etl::false_type {};
    template <typename T> struct is_rvalue_reference<T&&> : etl::true_type  {};
    template <typename T> inline constexpr bool is_rvalue_reference_v = etl::is_rvalue_reference<T>::value;

    /// is_reference
    template <typename T> struct is_reference 
        : etl::integral_constant<bool, etl::is_lvalue_reference_v<T> || etl::is_rvalue_reference_v<T>> {};
    template <typename T> inline constexpr bool is_reference_v = etl::is_reference<T>::value;

    /// remove reference
    template <typename T> struct remove_reference       { typedef T type; };
    template <typename T> struct remove_reference<T&>   { typedef T type; };
    template <typename T> struct remove_reference<T&&>  { typedef T type; };
    template <typename T> using remove_reference_t = typename etl::remove_reference<T>::type;

    /// add_rvalue_reference
    template <typename T> struct add_rvalue_reference       { typedef T&&  type; };
    template <typename T> struct add_rvalue_reference<T&>   { typedef T&   type; };
    template <typename T> struct add_rvalue_reference<T&&>  { typedef T&&  type; };
    template <>           struct add_rvalue_reference<void> { typedef void type; };
    template <typename T> using add_rvalue_reference_t = typename etl::add_rvalue_reference<T>::type;

    /// declval
    template <typename T> add_rvalue_reference_t<T> declval() noexcept;

    /// is_pointer
    template <typename T> struct is_pointer     : etl::false_type {};
    template <typename T> struct is_pointer<T*> : etl::true_type {};
    template <typename T> inline constexpr bool is_pointer_v = etl::is_pointer<T>::value;

    /// remove_pointer
    template <typename T> struct remove_pointer                          { typedef T type; };
    template <typename T> struct remove_pointer<T*>                      { typedef T type; };
    template <typename T> struct remove_pointer<const T*>                { typedef const T type; };
    template <typename T> struct remove_pointer<volatile T*>             { typedef volatile T type; };
    template <typename T> struct remove_pointer<const volatile T*>       { typedef const volatile T type; };
    template <typename T> struct remove_pointer<T* const>                { typedef T type; };
    template <typename T> struct remove_pointer<const T* const>          { typedef const T type; };
    template <typename T> struct remove_pointer<volatile T* const>       { typedef volatile T type; };
    template <typename T> struct remove_pointer<const volatile T* const> { typedef const volatile T type; };
    template <typename T> using remove_pointer_t = typename etl::remove_pointer<T>::type;

    /// add_pointer
    template <typename T> struct add_pointer { typedef typename etl::remove_reference<T>::type* type; };
    template <typename T> using add_pointer_t = typename etl::add_pointer<T>::type;

    /// is_const
    template <typename T> struct is_const                   : etl::false_type {};
    template <typename T> struct is_const<const T>          : etl::true_type  {};
    template <typename T> struct is_const<const volatile T> : etl::true_type  {};
    template <typename T> inline constexpr bool is_const_v = etl::is_const<T>::value;

    /// remove_const
    template <typename T> struct remove_const           { typedef T type; };
    template <typename T> struct remove_const<const T>  { typedef T type; };
    template <typename T> using remove_const_t = typename etl::remove_const<T>::type;

    /// add_const
    template <typename T> struct add_const          { typedef const T type; };
    template <typename T> struct add_const<const T> { typedef const T type; };
    template <typename T> using add_const_t = typename etl::add_const<T>::type;

    /// is_volatile
    template <typename T> struct is_volatile                   : etl::false_type {};
    template <typename T> struct is_volatile<volatile T>       : etl::true_type  {};
    template <typename T> struct is_volatile<const volatile T> : etl::true_type  {};
    template <typename T> inline constexpr bool is_volatile_v = etl::is_volatile<T>::value;

    /// remove_volatile
    template <typename T> struct remove_volatile             { typedef T type; };
    template <typename T> struct remove_volatile<volatile T> { typedef T type; };
    template <typename T> using remove_volatile_t = typename etl::remove_volatile<T>::type;

    /// add_volatile
    template <typename T> struct add_volatile             { typedef volatile T type; };
    template <typename T> struct add_volatile<volatile T> { typedef volatile T type; };
    template <typename T> using add_volatile_t = typename etl::add_volatile<T>::type;

    /// remove_const_volatile
    template <typename T> struct remove_const_volatile { typedef etl::remove_volatile_t<etl::remove_const_t<T>> type; };
    template <typename T> using remove_const_volatile_t = typename etl::remove_const_volatile<T>::type;

    /// add_const_volatile
    template <typename T> struct add_const_volatile { typedef etl::add_volatile_t<etl::add_const_t<T>> type; };
    template <typename T> using add_const_volatile_t = typename etl::add_const_volatile<T>::type;

    /// remove_const_volatile_ref
    template <typename T> struct remove_const_volatile_ref { typedef etl::remove_const_volatile_t<etl::remove_reference_t<T>> type; };
    template <typename T> using remove_const_volatile_ref_t = typename etl::remove_const_volatile_ref<T>::type;

    /// enable_if
    template <bool B, typename T = void> struct enable_if {};
    template <        typename T       > struct enable_if<true, T> { typedef T type; };
    template <bool B, typename T = void> using enable_if_t = typename etl::enable_if<B, T>::type;

    /// disable_if
    template <bool B, typename T = void> struct disable_if           {};
    template <        typename T       > struct disable_if<false, T> { typedef T type; };
    template <bool B, typename T = void> using disable_if_t = typename etl::disable_if<B, T>::type;

    /// conditional
    template <bool B, typename T, typename F> struct conditional              { typedef T type; };
    template <        typename T, typename F> struct conditional<false, T, F> { typedef F type; };
    template <bool B, typename T, typename F> using conditional_t = typename etl::conditional<B, T, F>::type;

    /// size comparison
    template <typename T1, typename T2> struct is_same_size    { static const bool value = sizeof (T1) == sizeof (T2); };
    template <typename T1, typename T2> struct is_bigger_size  { static const bool value = sizeof (T1) > sizeof (T2); };
    template <typename T1, typename T2> struct is_smaller_size { static const bool value = sizeof (T1) < sizeof (T2); };

    template <typename T1, typename T2> inline constexpr bool is_same_size_v    = etl::is_same_size<T1, T2>::value;
    template <typename T1, typename T2> inline constexpr bool is_bigger_size_v  = etl::is_bigger_size<T1, T2>::value;
    template <typename T1, typename T2> inline constexpr bool is_smaller_size_v = etl::is_same_size<T1, T2>::value;

    template <typename T1, typename T2> struct biggest_size {
        typedef etl::conditional_t<etl::is_same_size_v<T1, T2> || etl::is_bigger_size_v<T1, T2>, T1, T2> type;
    };
    template <typename T1, typename T2> struct smallest_size {
        typedef etl::conditional_t<etl::is_same_size_v<T1, T2> || etl::is_smaller_size_v<T1, T2>, T1, T2> type;
    };

    template <typename T1, typename T2> using biggest_size_t  = typename etl::biggest_size<T1, T2>::type;
    template <typename T1, typename T2> using smallest_size_t = typename etl::smallest_size<T1, T2>::type;

    /// is_same
    template <typename T1, typename T2> struct is_same : false_type {};
    template <typename T>               struct is_same<T, T> : true_type {};
    template <typename T1, typename T2> inline constexpr bool is_same_v = etl::is_same<T1, T2>::value;

    /// is_signed
    template <typename T> struct is_signed     : etl::false_type {};
    template <> struct is_signed<char>         : etl::bool_constant<(char(255) < 0)> {};
    template <> struct is_signed<wchar_t>      : etl::bool_constant<(wchar_t(-1) < wchar_t(0))> {};
    template <> struct is_signed<signed char>  : etl::true_type {};
    template <> struct is_signed<short>        : etl::true_type {};
    template <> struct is_signed<int>          : etl::true_type {};
    template <> struct is_signed<long>         : etl::true_type {};
    template <> struct is_signed<long long>    : etl::true_type {};
    template <> struct is_signed<float>        : etl::true_type {};
    template <> struct is_signed<double>       : etl::true_type {};
    template <> struct is_signed<long double>  : etl::true_type {};
    template <typename T> struct is_signed<const T>          : etl::is_signed<T> {};
    template <typename T> struct is_signed<volatile T>       : etl::is_signed<T> {};
    template <typename T> struct is_signed<const volatile T> : etl::is_signed<T> {};
    template <typename T> inline constexpr bool is_signed_v = etl::is_signed<T>::value;

    /// is_unsigned
    template <typename T> struct is_unsigned           : etl::false_type {};
    template <> struct is_unsigned<bool>               : etl::true_type {};
    template <> struct is_unsigned<char>               : etl::bool_constant<(char(255) > 0)> {};
    template <> struct is_unsigned<unsigned char>      : etl::true_type {};
    template <> struct is_unsigned<wchar_t>            : etl::bool_constant<(wchar_t(-1) > wchar_t(0))> {};
    template <> struct is_unsigned<unsigned short>     : etl::true_type {};
    template <> struct is_unsigned<unsigned int>       : etl::true_type {};
    template <> struct is_unsigned<unsigned long>      : etl::true_type {};
    template <> struct is_unsigned<unsigned long long> : etl::true_type {};
    template <typename T> struct is_unsigned<const T>          : etl::is_unsigned<T> {};
    template <typename T> struct is_unsigned<volatile T>       : etl::is_unsigned<T> {};
    template <typename T> struct is_unsigned<const volatile T> : etl::is_unsigned<T> {};
    template <typename T> inline constexpr bool is_unsigned_v = etl::is_unsigned<T>::value;

    /// add_unsigned
    template <typename T> struct add_unsigned  { typedef T type; };
    template <> struct add_unsigned<char>      { typedef unsigned char type; };
    template <> struct add_unsigned<short>     { typedef unsigned short type; };
    template <> struct add_unsigned<int>       { typedef unsigned int type; };
    template <> struct add_unsigned<long>      { typedef unsigned long type; };
    template <> struct add_unsigned<long long> { typedef unsigned long long type; };
    template <typename T> using add_unsigned_t = typename etl::add_unsigned<T>::type;
    template <typename T> struct add_unsigned<const T>          : etl::add_const<add_unsigned_t<T>> {};
    template <typename T> struct add_unsigned<volatile T>       : etl::add_volatile<add_unsigned_t<T>> {};
    template <typename T> struct add_unsigned<const volatile T> : etl::add_const_volatile<add_unsigned_t<T>> {};

    /// remove_unsigned
    template <typename T> struct remove_unsigned           { typedef T type; };
    template <> struct remove_unsigned<unsigned char>      { typedef char type; };
    template <> struct remove_unsigned<unsigned short>     { typedef short type; };
    template <> struct remove_unsigned<unsigned int>       { typedef int type; };
    template <> struct remove_unsigned<unsigned long>      { typedef long type; };
    template <> struct remove_unsigned<unsigned long long> { typedef long long type; };
    template <typename T> using remove_unsigned_t = typename etl::remove_unsigned<T>::type;
    template <typename T> struct remove_unsigned<const T>          : etl::add_const<remove_unsigned_t<T>> {};
    template <typename T> struct remove_unsigned<volatile T>       : etl::add_volatile<remove_unsigned_t<T>> {};
    template <typename T> struct remove_unsigned<const volatile T> : etl::add_const_volatile<remove_unsigned_t<T>> {};

    /// is_integral
    template <typename T> struct is_integral            : etl::false_type {};
    template <> struct is_integral<bool>                : etl::true_type {};
    template <> struct is_integral<char>                : etl::true_type {};
    template <> struct is_integral<unsigned char>       : etl::true_type {};
    template <> struct is_integral<signed char>         : etl::true_type {};
    template <> struct is_integral<wchar_t>             : etl::true_type {};
    template <> struct is_integral<short>               : etl::true_type {};
    template <> struct is_integral<unsigned short>      : etl::true_type {};
    template <> struct is_integral<int>                 : etl::true_type {};
    template <> struct is_integral<unsigned int>        : etl::true_type {};
    template <> struct is_integral<long>                : etl::true_type {};
    template <> struct is_integral<unsigned long>       : etl::true_type {};
    template <> struct is_integral<long long>           : etl::true_type {};
    template <> struct is_integral<unsigned long long>  : etl::true_type {};
    template <typename T> struct is_integral<const T>           : etl::is_integral<T> {};
    template <typename T> struct is_integral<volatile T>        : etl::is_integral<T> {};
    template <typename T> struct is_integral<const volatile T>  : etl::is_integral<T> {};
    template <typename T> inline constexpr bool is_integral_v           = etl::is_integral<T>::value;
    template <typename T> inline constexpr bool is_signed_integral_v    = etl::is_integral<T>::value && etl::is_signed<T>::value;
    template <typename T> inline constexpr bool is_unsigned_integral_v  = etl::is_integral<T>::value && etl::is_unsigned<T>::value;

    /// is_floating_point
    template <typename T> struct is_floating_point    : etl::false_type {};
    template <> struct is_floating_point<float>       : etl::true_type {};
    template <> struct is_floating_point<double>      : etl::true_type {};
    template <> struct is_floating_point<long double> : etl::true_type {};
    template <typename T> struct is_floating_point<const T>          : etl::is_floating_point<T> {};
    template <typename T> struct is_floating_point<volatile T>       : etl::is_floating_point<T> {};
    template <typename T> struct is_floating_point<const volatile T> : etl::is_floating_point<T> {};
    template <typename T> inline constexpr bool is_floating_point_v = etl::is_floating_point<T>::value;

    /// is_void
    template <typename T> struct is_void       : etl::false_type {};
    template <>           struct is_void<void> : etl::true_type {};
    template <typename T> inline static const bool is_void_v = etl::is_void<T>::value;

    /// is_arithmetic
    template <typename T> struct is_arithmetic : etl::bool_constant<etl::is_integral_v<T> || etl::is_floating_point_v<T>> {};
    template <typename T> inline static const bool is_arithmetic_v = etl::is_arithmetic<T>::value;

    /// is_fundamental
    template <typename T> struct is_fundamental : etl::bool_constant<etl::is_arithmetic_v<T> || etl::is_void_v<T>> {};
    template <typename T> inline static const bool  is_fundamental_v = etl::is_fundamental<T>::value;

    /// is_compound
    template <typename T> struct is_compound : etl::bool_constant<! etl::is_fundamental<T>::value> {};
    template <typename T> inline static const bool is_compound_v = etl::is_compound<T>::value;

    /// is_true_array
    template <typename T> struct is_true_array                 : etl::false_type {};
    template <typename T> struct is_true_array<T[]>            : etl::true_type {};
    template <typename T, size_t N> struct is_true_array<T[N]> : etl::true_type {};
    template <typename T> inline constexpr bool is_true_array_v = etl::is_true_array<T>::value;

    /// is_array
    template <typename T> struct is_array                 : etl::false_type {};
    template <typename T> struct is_array<T[]>            : etl::true_type {};
    template <typename T, size_t N> struct is_array<T[N]> : etl::true_type {};
    template <typename T> inline constexpr bool is_array_v = etl::is_array<T>::value;

    /// is_string
    template <typename T> struct is_string              : etl::false_type {};
    template <> struct is_string<char[]>                : etl::true_type {};
    template <> struct is_string<const char[]>          : etl::true_type {};
    template <> struct is_string<volatile char[]>       : etl::true_type {};
    template <> struct is_string<const volatile char[]> : etl::true_type {};
    template <size_t N> struct is_string<char[N]>                : etl::true_type {};
    template <size_t N> struct is_string<const char[N]>          : etl::true_type {};
    template <size_t N> struct is_string<volatile char[N]>       : etl::true_type {};
    template <size_t N> struct is_string<const volatile char[N]> : etl::true_type {};
    template <typename T> inline constexpr bool is_string_v = etl::is_string<T>::value;

    /// is_function
    template <typename T> struct is_function : etl::false_type {};
    template <typename R, typename... Args> struct is_function<R(Args...)> : etl::true_type {};
    template <typename T> inline constexpr bool is_function_v = etl::is_function<T>::value;

    /// is_function_pointer
    template <typename T> struct is_function_pointer : etl::false_type {};
    template <typename R, typename... Args> struct is_function_pointer<R(*)(Args...)> : etl::true_type {};
    template <typename T> inline constexpr bool is_function_pointer_v = etl::is_function_pointer<T>::value;

    /// is_function_reference
    template <typename T> struct is_function_reference : etl::false_type {};
    template <typename R, typename... Args> struct is_function_reference<R(&)(Args...)> : etl::true_type {};
    template <typename T> inline constexpr bool is_function_reference_v = etl::is_function_reference<T>::value;
   
    /// is_functor
    template <typename T>
    struct is_functor {
        template <typename U> static auto test(U* p) -> decltype(&U::operator(), void(), etl::true_type());
        template <typename U> static auto test(...) -> etl::false_type; // NOLINT
        static constexpr bool value = decltype(test<etl::remove_reference_t<T>>(nullptr))::value;
    };
    template <typename T> inline constexpr bool is_functor_v = etl::is_functor<T>::value;

    /// extent
    template <typename T, size_t N = 0> struct extent : etl::integral_constant<size_t, 0> {};
    template <typename T, size_t N> struct extent<T[], N> : etl::integral_constant<size_t, extent<T, N - 1>::value> {};
    template <typename T, size_t I> struct extent<T[I], 0> : etl::integral_constant<size_t, I> {};
    template <typename T, size_t I, size_t N> struct extent<T[I], N> : etl::extent<T, N - 1> {};
    template <typename T, size_t N = 0> inline constexpr size_t extent_v = etl::extent<T, N>::value;

    /// remove_extent
    template <typename T> struct remove_extent { typedef T type; };
    template <typename T> struct remove_extent<T[]> { typedef T type; };
    template <typename T, size_t N> struct remove_extent<T[N]> { typedef T type; };
    template <typename T> using remove_extent_t = typename etl::remove_extent<T>::type;

    /// decay
    template <typename T>
    struct decay {
        typedef etl::remove_reference_t<T> U;
        typedef etl::conditional_t<etl::is_true_array_v<U>, etl::remove_extent_t<U>*, etl::remove_const_volatile_t<U>> type;
    };
    template <typename T> using decay_t = typename decay<T>::type;

    /// integer sequence
    template<typename T, T... i>
    struct integer_sequence {
        typedef T value_type;
        static constexpr size_t size() noexcept { return sizeof...(i); }
    };

    template <typename T, T Begin, T End, T... Is>
    struct helper_make_index_sequence {
        using type = typename etl::helper_make_index_sequence<T, Begin, End-1, End-1, Is...>::type;
    };

    template <typename T, T Begin, T... Is>
    struct helper_make_index_sequence<T, Begin, Begin, Is...> {
        using type = etl::integer_sequence<size_t, Is...>;
    };

    template <size_t... i>
    using index_sequence = etl::integer_sequence<size_t, i...>;

    template <size_t Begin, size_t End>
    using make_range_sequence = typename etl::helper_make_index_sequence<size_t, Begin, End>::type;

    template <size_t N>
    using make_index_sequence = typename etl::helper_make_index_sequence<size_t, 0, N>::type;

    template <typename... T> using index_sequence_for = etl::make_index_sequence<sizeof...(T)>;

    /// common type
    template <typename...> struct common_type {};

    template <typename T, typename U>
    struct common_type<T, U> { using type = etl::remove_reference_t<decltype(true ? etl::declval<T>() : etl::declval<U>())>; };

    template <typename T, typename... Ts>
    struct common_type<T, Ts...> {
        using type = etl::conditional_t<sizeof...(Ts) == 0, T, typename etl::common_type<T, typename etl::common_type<Ts...>::type>::type>;
    };

    template <typename T, typename... Ts> using common_type_t = typename etl::common_type<T, Ts...>::type;

    /// is_convertible
    template <typename T, typename U>
    struct is_convertible {
        static void test(U);
        template <typename V> static auto check(int) -> decltype(test(etl::declval<V>()), void(), etl::true_type());
        template <typename V> static auto check(...) -> etl::false_type;
        static constexpr bool value = decltype(check<T>(0))::value;
    };
    template <typename T, typename U> inline constexpr bool is_convertible_v = etl::is_convertible<T, U>::value;

    // is_copy_constructible
    template <typename T>
    struct is_copy_constructible {
        template <typename U, typename = decltype(U(etl::declval<const U&>()))> static auto check(int) -> etl::true_type;
        template <typename U> static auto check(...) -> etl::false_type;
        static constexpr bool value = decltype(check<T>(0))::value;
    };
    template <typename T, size_t N> struct is_copy_constructible<T[N]> : etl::false_type {};
    template <typename T> struct is_copy_constructible<T&> : etl::false_type {};
    template <typename T> inline constexpr bool is_copy_constructible_v = etl::is_copy_constructible<T>::value;

    // is_move_constructible
    template <typename T>
    struct is_move_constructible {
        template <typename U, typename = decltype(U(etl::declval<U&&>()))> static auto check(int) -> etl::true_type;
        template <typename U> static auto check(...) -> etl::false_type;
        static constexpr bool value = decltype(check<T>(0))::value;
    };
    template <typename T, size_t N> struct is_move_constructible<T[N]> : etl::false_type {};
    template <typename T> struct is_move_constructible<T&> : etl::false_type {};
    template <> struct is_move_constructible<void> : etl::false_type {};
    template <typename T> struct is_move_constructible<const T> : etl::is_move_constructible<T> {};
    template <typename T> struct is_move_constructible<volatile T> : etl::is_move_constructible<T> {};
    template <typename T> struct is_move_constructible<const volatile T> : etl::is_move_constructible<T> {};
    template <typename T> inline constexpr bool is_move_constructible_v = etl::is_move_constructible<T>::value;

    // is_copy_assignable
    template <typename T>
    struct is_copy_assignable {
        template <typename U, typename = decltype(etl::declval<U&>() = etl::declval<const U&>())> static auto check(int) -> etl::true_type;
        template <typename U> static auto check(...) -> etl::false_type;
        static constexpr bool value = decltype(check<T>(0))::value;
    };
    template <typename T, size_t N> struct is_copy_assignable<T[N]> : etl::false_type {};
    template <typename T> struct is_copy_assignable<T&> : etl::false_type {};
    template <> struct is_copy_assignable<void> : etl::false_type {};
    template <typename T> struct is_copy_assignable<const T> : etl::false_type {};
    template <typename T> struct is_copy_assignable<volatile T> : etl::false_type {};
    template <typename T> struct is_copy_assignable<const volatile T> : etl::false_type {};
    template <typename T> inline constexpr bool is_copy_assignable_v = etl::is_copy_assignable<T>::value;

    // is_move_assignable
    template <typename T>
    struct is_move_assignable {
        template <typename U, typename = decltype(etl::declval<U&>() = etl::declval<U&&>())> static auto check(int) -> etl::true_type;
        template <typename U> static auto check(...) -> etl::false_type;
        static constexpr bool value = decltype(check<T>(0))::value;
    };
    template <typename T, size_t N> struct is_move_assignable<T[N]> : etl::false_type {};
    template <typename T> struct is_move_assignable<T&> : etl::false_type {};
    template <> struct is_move_assignable<void> : etl::false_type {};
    template <typename T> struct is_move_assignable<const T> : etl::false_type {};
    template <typename T> struct is_move_assignable<volatile T> : etl::false_type {};
    template <typename T> struct is_move_assignable<const volatile T> : etl::false_type {};
    template <typename T> inline constexpr bool is_move_assignable_v = etl::is_move_assignable<T>::value;

    // is_destructible
    template <typename T> struct is_destructible : etl::bool_constant<etl::is_compound_v<T> && !etl::is_pointer_v<T>> {};
    template <typename T> inline constexpr bool is_destructible_v = etl::is_destructible<T>::value;

    // has_empty_constructor
    template <typename T>
    struct has_empty_constructor {
        template <typename U, typename = decltype(U())> static auto check(int) -> etl::true_type;
        template <typename U> static auto check(...) -> etl::false_type;
        static constexpr bool value = decltype(check<T>(0))::value;
    };
    template <typename T> inline constexpr bool has_empty_constructor_v = etl::has_empty_constructor<T>::value;

    /// a meta-function that always yields void, used for detecting valid types.
    template <typename...> using void_t = void;

    /// has_prefix_increment
    template <typename T, typename = void> struct has_prefix_increment : etl::bool_constant<etl::is_pointer_v<T>> {};
    template <typename T> struct has_prefix_increment<T, etl::void_t<decltype(++etl::declval<T>())>> : etl::true_type {};
    template <typename T> inline constexpr bool has_prefix_increment_v = etl::has_prefix_increment<T>::value;

    /// has_postfix_increment
    template <typename T, typename = void> struct has_postfix_increment : etl::bool_constant<etl::is_pointer_v<T>> {};
    template <typename T> struct has_postfix_increment<T, etl::void_t<decltype(etl::declval<T>()++)>> : etl::true_type {};
    template <typename T> inline constexpr bool has_postfix_increment_v = etl::has_postfix_increment<T>::value;

    /// has_prefix_decrement
    template <typename T, typename = void> struct has_prefix_decrement : etl::bool_constant<etl::is_pointer_v<T>> {};
    template <typename T> struct has_prefix_decrement<T, etl::void_t<decltype(--etl::declval<T>())>> : etl::true_type {};
    template <typename T> inline constexpr bool has_prefix_decrement_v = etl::has_prefix_decrement<T>::value;

    /// has_postfix_decrement
    template <typename T, typename = void> struct has_postfix_decrement : etl::bool_constant<etl::is_pointer_v<T>> {};
    template <typename T> struct has_postfix_decrement<T, etl::void_t<decltype(etl::declval<T>()--)>> : etl::true_type {};
    template <typename T> inline constexpr bool has_postfix_decrement_v = etl::has_postfix_decrement<T>::value;

    /// has_operator_plus_int
    template <typename T, typename = void> struct has_operator_plus_int : etl::bool_constant<etl::is_pointer_v<T>> {};
    template <typename T> struct has_operator_plus_int<T, etl::void_t<decltype(etl::declval<T>() + 1)>> : etl::true_type {};
    template <typename T> inline constexpr bool has_operator_plus_int_v = etl::has_operator_plus_int<T>::value;

    /// has_operator_minus_int
    template <typename T, typename = void> struct has_operator_minus_int : etl::bool_constant<etl::is_pointer_v<T>> {};
    template <typename T> struct has_operator_minus_int<T, etl::void_t<decltype(etl::declval<T>() - 1)>> : etl::true_type {};
    template <typename T> inline constexpr bool has_operator_minus_int_v = etl::has_operator_minus_int<T>::value;

    /// has_operator_minus
    template <typename T, typename = void> struct has_operator_minus : etl::bool_constant<etl::is_pointer_v<T>> {};
    template <typename T> struct has_operator_minus<T, etl::void_t<decltype(etl::declval<T>() - etl::declval<T>())>> : etl::true_type {};
    template <typename T> inline constexpr bool has_operator_minus_v = etl::has_operator_minus<T>::value;

    /// has_operator_dereference
    template <typename T, typename = void> struct has_operator_dereference : etl::bool_constant<etl::is_pointer_v<T>> {};
    template <typename T> struct has_operator_dereference<T, etl::void_t<decltype(*etl::declval<T>())>> : etl::true_type {};
    template <typename T> inline constexpr bool has_operator_dereference_v = etl::has_operator_dereference<T>::value;

    template <typename T, typename = void> struct has_operator_bool : etl::false_type {};
    template <typename T> struct has_operator_bool<T, etl::void_t<decltype(bool(etl::declval<T>()))>> : etl::true_type {};
    template <typename T> inline constexpr bool has_operator_bool_v = etl::has_operator_bool<T>::value;

    /// is_iterator
    template <typename T> struct is_iterator : etl::bool_constant<etl::has_prefix_increment_v<T> && etl::has_operator_dereference_v<T>> {};
    template <typename T> inline constexpr bool is_iterator_v = etl::is_iterator<T>::value;

    /// is_subscriptable
    template <typename T, typename = void> struct is_subscriptable : etl::false_type {};
    template <typename T> struct is_subscriptable<T, etl::void_t<decltype(etl::declval<T>()[0])>> : etl::true_type {};
    template <typename T> inline constexpr bool is_subscriptable_v = etl::is_subscriptable<T>::value;
}

/// trait helper
namespace Project::etl::detail {
    template <typename T, typename = void> struct trait_has_len : etl::false_type {};
    template <typename T> struct trait_has_len<T, etl::void_t<decltype(etl::declval<T>().len())>> : etl::true_type {};

    template <typename T, typename = void> struct trait_has_size : etl::false_type {};
    template <typename T> struct trait_has_size<T, etl::void_t<decltype(etl::declval<T>().size())>>  : etl::true_type {};

    template <typename T, typename = void> struct trait_has_begin_end : etl::false_type {};
    template <typename T> struct trait_has_begin_end<T, void_t<decltype(etl::declval<T>().begin()), decltype(etl::declval<T>().end())>> 
        : etl::true_type {};

    template <typename T, typename = void> struct trait_has_rbegin_rend : etl::false_type {};
    template <typename T> struct trait_has_rbegin_rend<T, void_t<decltype(etl::declval<T>().rbegin()), decltype(etl::declval<T>().rend())>> 
        : etl::true_type {};

    template <typename T, typename = void> struct trait_has_iter : etl::false_type {};
    template <typename T> struct trait_has_iter<T, void_t<decltype(etl::declval<T>().iter())>> : etl::true_type {};

    template <typename T, typename = void> struct trait_has_reversed : etl::false_type {};
    template <typename T> struct trait_has_reversed<T, void_t<decltype(etl::declval<T>().reversed())>> : etl::true_type {};

    template <typename T, typename = void> struct trait_has_empty : etl::false_type {};
    template <typename T> struct trait_has_empty<T, void_t<decltype(etl::declval<T>().empty())>> : etl::true_type {};
}

// trait methods
namespace Project::etl {
    // trait len
    template <typename T, typename = void> struct trait_len : etl::false_type {};

    // trait iterable
    template <typename T, typename = void> struct trait_iterable : etl::false_type {};

    // trait reverse iterable
    template <typename T, typename = void> struct trait_reverse_iterable : etl::false_type {};

    /// trait get<i>()
    template <size_t i, typename T, typename = void> struct trait_get_from_index : etl::false_type {};

    /// trait get<I>()
    template <typename I, typename T, typename = void> struct trait_get_from_type : etl::false_type {};

    /// trait get<i, j>()
    template <size_t i, size_t j, typename T, typename = void> struct trait_tuple_slice : etl::false_type {};

    /// trait next()
    template <typename T, typename = void> struct trait_generator : etl::false_type {};

    /// trait json serializer
    template <typename T, typename = void> struct trait_json_serializer : etl::false_type {};

    /// trait json deserializer
    template <typename T, typename = void> struct trait_json_deserializer : etl::false_type {};

    // TODO
    template <typename T, typename U, typename = void> struct trait_eq : etl::false_type {};

    // TODO
    template <typename T, typename U, typename = void> struct trait_swap : etl::false_type {};

    // TODO
    template <typename T, typename = void> struct trait_slice : etl::false_type {};

    /// is_etl_json
    template <typename T> struct is_etl_json : etl::false_type {};
    template <typename T> inline constexpr bool is_etl_json_v = etl::is_etl_json<T>::value;
}

namespace Project::etl::placeholder {
    template <typename T> struct Argument;
    template <typename T> struct is_arg : etl::false_type {};
    template <typename T> struct is_arg<Argument<T>> : etl::true_type {};
    template <typename T> struct is_arg<const Argument<T>> : etl::true_type {};
    template <typename T> struct is_arg<volatile Argument<T>> : etl::true_type {};
    template <typename T> struct is_arg<const volatile Argument<T>> : etl::true_type {};
    template <typename T> inline constexpr bool is_arg_v = etl::placeholder::is_arg<T>::value;
}

#endif //ETL_TYPE_TRAITS_H
