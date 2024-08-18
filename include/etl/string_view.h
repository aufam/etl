#ifndef ETL_STRING_VIEW_H
#define ETL_STRING_VIEW_H

#include <cstring> // strlen, strcpy, memset

#ifndef ETL_STRING_DEFAULT_SIZE
#define ETL_STRING_DEFAULT_SIZE 64
#endif

#ifndef ETL_SHORT_STRING_DEFAULT_SIZE
#define ETL_SHORT_STRING_DEFAULT_SIZE 16
#endif

#include "etl/algorithm.h"

namespace Project::etl {
    template <size_t N> class StringSplit;
    template <size_t N> class StringMatch;

    class StringView {
        template <size_t N> friend class StringSplit;
        template <size_t N> friend class StringMatch;

        const char* str;
        size_t length;

    public:
        /* constructors and assignments */

        /// empty constructor
        constexpr StringView() : str(nullptr), length(0) {}

        /// default constructor, notice that it doesn't check the '\0' termination
        constexpr StringView(const char* str, size_t length) : str(str), length(length) {}

        /// implicit constructor from C string
        constexpr StringView(const char* str) : str(str), length(calculate_length(str)) {}

        /// copy constructor
        constexpr StringView(const StringView& other) = default;

        /// copy assignment
        constexpr StringView& operator=(const StringView& other) = default;

        /// move constructor
        constexpr StringView(StringView&& other) noexcept : str(etl::exchange(other.str, nullptr)), length(etl::exchange(other.length, 0)) {}

        /// move assignment
        constexpr StringView& operator=(StringView&& other) noexcept {
            if (this == &other) return *this;
            str = etl::exchange(other.str, nullptr);
            length = etl::exchange(other.length, 0);
            return *this;
        }

        /* container related */

        /// return true if first character is not '\0'
        constexpr explicit operator bool() const { return length > 0 && str != nullptr && str[0] != '\0'; }

        /// string length, maximum is length - 1
        [[nodiscard]] constexpr size_t len() const { return length; }

        constexpr const char* data()   const { return str; }
        constexpr const char* begin()  const { return str; }
        constexpr const char* end()    const { return str + len(); }
        constexpr const char& front()  const { return str[0]; }
        constexpr const char& back()   const { auto l = len(); return str[l ? l - 1 : 0]; }

        constexpr auto iter() const { return str ? Iter(begin(), end(), 1) : Iter<const char*>(nullptr, nullptr, 0); }
        constexpr auto reversed() const { return str ? Iter(end() - 1, begin() - 1, -1) : Iter<const char*>(nullptr, nullptr, 0); }

        constexpr const char& operator[](int i) const {
            if (i < 0) i = len() + i;
            if (i < 0) return *static_cast<const char*>(nullptr); // error null dereference
            return str[i];
        }

        /* comparison */

        /// implementation of strncmp
        constexpr int compare(const StringView& other) const { 
            const auto min_len = etl::min(length, other.length);
            for (size_t i = 0; i < min_len; ++i) {
                if (str[i] < other.str[i]) {
                    return -1;
                } else if (str[i] > other.str[i]) {
                    return 1;
                } else if ((str[i] == '\0' && i + 1 == other.length) || 
                           (other.str[i] == '\0' && i + 1 == length)) {
                    // Both strings are equal up to this point, and s1 is shorter
                    return 0;
                }
            }
            return 0;
        }

        constexpr bool operator==(const StringView& other) const { 
            if (length == other.length) {
                return compare(other) == 0; 
            }
            return false;
        }
        
        constexpr bool operator!=(const StringView& other) const { 
            return !operator==(other); 
        }

        /* substring */

        /// create a substring
        /// @param start start index
        /// @param length desired length
        /// @return new StringView
        constexpr auto substr(int start, size_t length) const { return StringView(&operator[](start), length); }

        /// find substring inside the string
        constexpr size_t find(const StringView& substring) const {
            size_t res = 0;
            for (size_t i = 0; i < length; ++i) {
                bool check = true;
                for (size_t j = 0; j < substring.length; ++j) {
                    if (i + j >= length) {
                        return length;
                    }
                    
                    check &= str[i + j] == substring.str[j];
                    if (!check)
                        continue;
                }
                
                if (check) {
                    return res;
                }

                ++res;
            }
            return res;
        }

        /// check if a substring is inside this string
        constexpr bool contains(const StringView& substring) const { return find(substring) < length; }

        /* explicit conversion to int and to float */

        constexpr int to_int() const { 
            return to_int_helper(); 
        }
        
        constexpr int to_int_or(int value) const { 
            return (str && ((str[0] >= '0' && str[0] <= '9') || str[0] == '-'|| str[0] == '+')) 
                ? to_int_helper() 
                : value; 
        }
        
        constexpr float to_float() const { 
            return to_float_helper(); 
        }

        constexpr float to_float_or(float value) const { 
            return (str && ((str[0] >= '0' && str[0] <= '9') || str[0] == '-' || str[0] == '+' || str[0] == '.')) 
                ? to_float_helper() 
                : value; 
        }

        /* split and match */
        
        template <size_t N = ETL_SHORT_STRING_DEFAULT_SIZE>
        StringSplit<N> split(StringView separator = " ") const;

        template <size_t N = ETL_SHORT_STRING_DEFAULT_SIZE>
        StringMatch<N> match(StringView format, StringView separator = "%s") const;
    
    private:
        static constexpr size_t calculate_length(const char* text) {
            if (text == nullptr) return 0;
            size_t length = 0;
            while (text[length] != '\0') {
                ++length;
            }
            return length;
        }

        constexpr int to_int_helper(int result = 0, size_t index = 0) const {
            if (!str || index >= length)
                return result;
            
            if (str[0] == '-')
                return -substr(1, length - 1).to_int();

            if (str[0] == '+')
                return substr(1, length - 1).to_int();

            return (str[index] >= '0' && str[index] <= '9') 
                ? to_int_helper(result * 10 + (str[index] - '0'), index + 1) 
                : result;
        }

        constexpr int to_int_no_sign_helper(int result = 0, size_t index = 0) const {
            if (!str || index >= length)
                return result;

            return (str[index] >= '0' && str[index] <= '9') 
                ? to_int_no_sign_helper(result * 10 + (str[index] - '0'), index + 1) 
                : result;
        }
 
        constexpr float to_float_helper(float result = 0.0f, size_t index = 0) const {
            if (!str || index >= length)
                return result;
            
            if (str[0] == '-')
                return -substr(1, length - 1).to_float();

            if (str[0] == '+')
                return substr(1, length - 1).to_float();

            if (str[index] == '.') {
                auto sv = substr(index + 1, length - (index + 1));
                auto cn = sv.contiguous_number();
                auto res = result + float(sv.to_int_no_sign_helper()) / ten_to_the_power_of(cn);
                auto maxIndex = cn + index + 1;
                return maxIndex < length && (str[maxIndex] == 'e' || str[maxIndex] == 'E')
                    ? to_float_helper(res, maxIndex)
                    : res;
            }
            
            if (str[index] == 'e' || str[index] == 'E') {
                auto sv = substr(index + 1, length - (index + 1));
                auto power = sv.to_int();
                return result * ten_to_the_power_of(power);
            }
            
            return (str && index < length && ((str[index] >= '0' && str[index] <= '9') || str[index] == '.'))
                ? to_float_helper(result * 10 + (str[index] - '0'), index + 1)
                : result;
        }

        constexpr int contiguous_number(size_t index = 0) const {
            return (str && index < length && str[index] >= '0' && str[index] <= '9')
                ? contiguous_number(index + 1)
                : index;
        }

        constexpr static float ten_to_the_power_of(int power) {
            if (power == 0)
                return 1.0f;
            
            if (power < 0)
                return 1.0f / ten_to_the_power_of(-power);
            
            return 10 * ten_to_the_power_of(power - 1);
        }
    };

    constexpr bool operator==(const char* str, const StringView& other) { 
        return other.compare(str) == 0;
    }
    
    constexpr bool operator!=(const char* str, const StringView& other) { 
        return other != str;
    }

    /// create empty string view
    inline constexpr auto 
    string_view() { return StringView(); }
    
    /// create string view from C-string
    inline constexpr auto 
    string_view(const char* text) { return StringView(text); }
    
    /// create string view from C-string with length, it doesn't check the '\0' termination
    inline constexpr auto 
    string_view(const char* text, size_t length) { return StringView(text, length); }
    
    /// create string view from uint8_t*
    inline auto 
    string_view(const uint8_t* text) { return StringView(reinterpret_cast<const char*>(text)); }
    
    /// create string view from uint8_t* with length, it doesn't check the '\0' termination
    inline auto 
    string_view(const uint8_t* text, size_t length) { return StringView(reinterpret_cast<const char*>(text), length); }

    /// type traits
    template <> struct is_string<StringView> : true_type {};
    template <> struct is_string<const StringView> : true_type {};
    template <> struct is_string<volatile StringView> : true_type {};
    template <> struct is_string<const volatile StringView> : true_type {};

    template <> struct is_array<StringView> : true_type {};
    template <> struct is_array<const StringView> : true_type {};
    template <> struct is_array<volatile StringView> : true_type {};
    template <> struct is_array<const volatile StringView> : true_type {};

    template <> struct remove_extent<StringView> { typedef char type; };
    template <> struct remove_extent<volatile StringView> { typedef char type; };
    template <> struct remove_extent<const volatile StringView> { typedef char type; };

    template <size_t N> 
    class StringSplit {
        size_t argc = 0;    ///< number of arguments
        StringView argv[N] = {}; ///< array of argument values

    public: 
        /// default constructor
        explicit StringSplit(StringView text, StringView separator = " ", bool ignore_duplicate = false) {
            for (auto next_token = text; next_token.begin() < text.end() && argc < N;) {
                auto pos = next_token.find(separator);
                auto pos_max = pos + separator.length;

                if (pos > next_token.length) {
                    break;
                }

                if (ignore_duplicate || pos > 0) // pos == 0 means it is duplicate
                    argv[argc++] = next_token.substr(0, pos);

                next_token = next_token.substr(pos_max, next_token.length - pos_max);
            }
        }

        size_t len() const { return argc; }

        StringView* begin()  { return &argv[0]; }
        StringView* end()    { return &argv[argc]; }

        const StringView* begin()  const { return &argv[0]; }
        const StringView* end()    const { return &argv[argc]; }

        StringView operator[](int index) const { 
            if (index < 0) index = len() + index;
            if (index < 0) return nullptr;
            return argv[index];
         }

         template <size_t I>
         StringView get() const { return operator[](I); }
    };

    template <size_t N>
    StringSplit<N> StringView::split(StringView separator) const {
        return StringSplit<N>(*this, separator);
    }

    template <size_t N>
    class StringMatch {
        size_t argc = 0;    ///< number of arguments
        StringView argv[N] = {}; ///< array of argument values

    public: 
        explicit StringMatch(StringView text, StringView format, StringView separator = "%s") {
            if (format == separator) {
                argv[argc++] = text;
                return;
            }

            struct Res { const char* begin, * end; };
            Res res[N] = {};

            auto format_split = StringSplit<N>(format, separator, true);
            for (auto &token : format_split) {
                auto pos = text.find(token);
                auto pos_max = pos + token.length;

                if (pos >= text.length) {
                    break;
                }

                if (argc > 0) {
                    res[argc - 1].end = text.begin() + pos;
                }

                res[argc].begin = text.begin() + pos_max;
                res[argc].end = token.end() == format.end() ? text.begin() + pos_max : text.end();
                ++argc;

                text = text.substr(pos_max, text.length - pos_max);
            }

            for (size_t i = 0; i < argc; ++i) {
                auto [begin, end] = res[i];
                if (begin == end) {
                    argc = i;
                    break;
                }
                argv[i] = StringView(begin, end - begin);
            }
        }

        size_t len() const { return argc; }

        StringView* begin()  { return &argv[0]; }
        StringView* end()    { return &argv[argc]; }

        const StringView* begin()  const { return &argv[0]; }
        const StringView* end()    const { return &argv[argc]; }

        StringView operator[](int index) const { 
            if (index < 0) index = len() + index;
            if (index < 0) return nullptr;
            return argv[index];
         }

         template <size_t I>
         StringView get() const { return operator[](I); }
    };

    template <size_t N>
    StringMatch<N> StringView::match(StringView format, StringView separator) const {
        return StringMatch<N>(*this, format, separator);
    }
}

namespace Project::etl::literals {
    constexpr auto operator ""sv(const char* text, size_t length) {
        return StringView(text, length);
    }
}

template <size_t N>
struct std::tuple_size<Project::etl::StringSplit<N>> : Project::etl::integral_constant<size_t, N> {};

template <size_t Index, size_t N>
struct std::tuple_element<Index, Project::etl::StringSplit<N>> {
    using type = Project::etl::StringView;
};

template <size_t N>
struct std::tuple_size<Project::etl::StringMatch<N>> : Project::etl::integral_constant<size_t, N> {};

template <size_t Index, size_t N>
struct std::tuple_element<Index, Project::etl::StringMatch<N>> {
    using type = Project::etl::StringView;
};

#endif //ETL_STRING_VIEW_H
