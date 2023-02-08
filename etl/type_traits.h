#ifndef ETL_TYPE_TRAITS_H
#define ETL_TYPE_TRAITS_H

#include <ciso646>

constexpr size_t operator ""_k(unsigned long long val)  { return val * 1000; }
constexpr size_t operator ""_k(long double val)         { return size_t(val * 1000); }

constexpr size_t operator ""_M(unsigned long long val)  { return val * 1000_k; }
constexpr size_t operator ""_M(long double val)         { return size_t(val * 1000_k); }

constexpr size_t operator ""_G(unsigned long long val)  { return val * 1000_M; }
constexpr size_t operator ""_G(long double val)         { return size_t(val * 1000_M); }

namespace Project::etl {

    /// remove reference @{
    template <typename T> struct remove_reference       { typedef T type; };
    template <typename T> struct remove_reference<T&>   { typedef T type; };
    template <typename T> struct remove_reference<T&&>  { typedef T type; };
    /// @}

    /// enable_if @{
    template <bool B, typename T = void> struct enable_if {};
    template <typename T> struct enable_if<true, T> { typedef T type; };

    template <bool B, typename T = void> using enable_if_t = typename enable_if<B, T>::type;
    /// @}

    /// enable_if_else @{
    template <bool B, typename T1, typename T2> struct enable_if_else;
    template <typename T1, typename T2> struct enable_if_else<true, T1, T2> { typedef T1 type; };
    template <typename T1, typename T2> struct enable_if_else<false, T1, T2> { typedef T2 type; };

    template <bool B, typename T1, typename T2> using enable_if_else_t = typename enable_if_else<B, T1, T2>::type;
    /// @}

    /// integral constant @{
    template <typename T, const T VALUE> struct integral_constant {
        typedef T value_type;
        typedef integral_constant<T, VALUE> type;
        static const T value = VALUE;
    };

    typedef integral_constant<bool, false> false_type;
    typedef integral_constant<bool, true>  true_type;

    template <bool B> using bool_constant = integral_constant<bool, B>;
    template <typename T, const T VALUE> const T integral_constant<T, VALUE>::value;
    /// @}

    /// size comparison @{
    template <typename T1, typename T2> struct is_same_size    { static const bool value = sizeof (T1) == sizeof (T2); };
    template <typename T1, typename T2> struct is_bigger_size  { static const bool value = sizeof (T1) > sizeof (T2); };
    template <typename T1, typename T2> struct is_smaller_size { static const bool value = sizeof (T1) < sizeof (T2); };

    template <typename T1, typename T2> inline constexpr bool is_same_size_v    = is_same_size<T1, T2>::value;
    template <typename T1, typename T2> inline constexpr bool is_bigger_size_v  = is_bigger_size<T1, T2>::value;
    template <typename T1, typename T2> inline constexpr bool is_smaller_size_v = is_same_size<T1, T2>::value;

    template <typename T1, typename T2> struct biggest_size {
        typedef enable_if_else_t<is_same_size_v<T1, T2> or is_bigger_size_v<T1, T2>, T1, T2> type;
    };
    template <typename T1, typename T2> struct smallest_size {
        typedef enable_if_else_t<is_same_size_v<T1, T2> or is_smaller_size_v<T1, T2>, T1, T2> type;
    };

    template <typename T1, typename T2> using biggest_size_t = typename biggest_size<T1, T2>::type;
    template <typename T1, typename T2> using smallest_size_t = typename smallest_size<T1, T2>::type;
    /// @}

    /// is_same @{
    template <typename T1, typename T2> struct is_same : public false_type {};
    template <typename T> struct is_same<T, T> : public true_type {};

    template <typename T1, typename T2> inline constexpr bool is_same_v = is_same<T1, T2>::value;
    /// @}

    /// is_signed @{
    template <typename T> struct is_signed : false_type {};
    template <> struct is_signed<char> : bool_constant<(char(255) < 0)> {};
    template <> struct is_signed<wchar_t> : public bool_constant<static_cast<bool>(wchar_t(-1) < wchar_t(0))> {};
    template <> struct is_signed<signed char> : true_type {};
    template <> struct is_signed<short> : true_type {};
    template <> struct is_signed<int> : true_type {};
    template <> struct is_signed<long> : true_type {};
    template <> struct is_signed<long long> : true_type {};
    template <> struct is_signed<float> : true_type {};
    template <> struct is_signed<double> : true_type {};
    template <> struct is_signed<long double> : true_type {};
    template <typename T> struct is_signed<const T> : is_signed<T> {};
    template <typename T> struct is_signed<volatile T> : is_signed<T> {};
    template <typename T> struct is_signed<const volatile T> : is_signed<T> {};

    template <typename T> inline constexpr bool is_signed_v = is_signed<T>::value;
    /// @}

    /// is_unsigned @{
    template <typename T> struct is_unsigned : false_type {};
    template <> struct is_unsigned<bool> : true_type {};
    template <> struct is_unsigned<char> : bool_constant<(char(255) > 0)> {};
    template <> struct is_unsigned<unsigned char> : true_type {};
    template <> struct is_unsigned<wchar_t> : public bool_constant<(wchar_t(-1) > wchar_t(0))> {};
    template <> struct is_unsigned<unsigned short> : true_type {};
    template <> struct is_unsigned<unsigned int> : true_type {};
    template <> struct is_unsigned<unsigned long> : true_type {};
    template <> struct is_unsigned<unsigned long long> : true_type {};
    template <typename T> struct is_unsigned<const T> : is_unsigned<T> {};
    template <typename T> struct is_unsigned<volatile T> : is_unsigned<T> {};
    template <typename T> struct is_unsigned<const volatile T> : is_unsigned<T> {};

    template <typename T> inline constexpr bool is_unsigned_v = is_unsigned<T>::value;
    /// @}

    /// is_integral @{
    template <typename T> struct is_integral : false_type {};
    template <> struct is_integral<bool> : true_type {};
    template <> struct is_integral<char> : true_type {};
    template <> struct is_integral<unsigned char> : true_type {};
    template <> struct is_integral<signed char> : true_type {};
    template <> struct is_integral<wchar_t> : true_type {};
    template <> struct is_integral<short> : true_type {};
    template <> struct is_integral<unsigned short> : true_type {};
    template <> struct is_integral<int> : true_type {};
    template <> struct is_integral<unsigned int> : true_type {};
    template <> struct is_integral<long> : true_type {};
    template <> struct is_integral<unsigned long> : true_type {};
    template <> struct is_integral<long long> : true_type {};
    template <> struct is_integral<unsigned long long> : true_type {};
    template <typename T> struct is_integral<const T> : is_integral<T> {};
    template <typename T> struct is_integral<volatile T> : is_integral<T> {};
    template <typename T> struct is_integral<const volatile T> : is_integral<T> {};

    template <typename T> inline constexpr bool is_integral_v = is_integral<T>::value;
    template <typename T> inline constexpr bool is_signed_integral_v = is_integral<T>::value and is_signed<T>::value;
    template <typename T> inline constexpr bool is_unsigned_integral_v = is_integral<T>::value and is_unsigned<T>::value;
    /// @}

    /// is_floating_point
    template <typename T> struct is_floating_point : false_type {};
    template <> struct is_floating_point<float> : true_type {};
    template <> struct is_floating_point<double> : true_type {};
    template <> struct is_floating_point<long double> : true_type {};
    template <typename T> struct is_floating_point<const T> : is_floating_point<T> {};
    template <typename T> struct is_floating_point<volatile T> : is_floating_point<T> {};
    template <typename T> struct is_floating_point<const volatile T> : is_floating_point<T> {};

    template <typename T> inline constexpr bool is_floating_point_v = is_floating_point<T>::value;
    /// @}

    /// is_array
    template <typename T> struct is_array : false_type {};
    template <typename T> struct is_array<T[]> : true_type {};
    template <typename T, size_t N> struct is_array<T[N]> : true_type {};

    template <typename T> inline constexpr bool is_array_v = is_array<T>::value;
    /// @}
}

#endif //ETL_TYPE_TRAITS_H
