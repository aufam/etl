#ifndef ETL_JSON_H
#define ETL_JSON_H

#include "etl/string_view.h"

namespace Project::etl {

    /// simple json parser
    class Json {
    public:
        enum class Type { UNKNOWN, DICTIONARY, DICTIONARY_ITEM, LIST, STRING, NUMBER, BOOL, NULL_, };

        /// empty constructor
        constexpr Json() : text() {}

        /// parse
        static constexpr Json parse(StringView text) { return Json::parse(text, Type::UNKNOWN); }

        /// return false is type is unknown
        constexpr explicit operator bool() const { return type != Type::UNKNOWN; }

        /// retrieve next JSON object
        constexpr Json next() const { return Json::parse(next_text, parent_type); }

        /// dump JSON data to a string view
        constexpr StringView dump() const { return text; }

        /// retrieve the value associated with a given key in the JSON dictionary. 
        /// if the JSON object is not a dictionary or the key is not found, return empty JSON 
        constexpr Json operator[](StringView key) const {
            if (!is_iterable()) {
                return Json::error("JSON object is not iterable");
            }

            for (auto json : *this) if (json.key == key) {
                return Json::parse(json.value, Type::DICTIONARY_ITEM);
            }
            return Json::error("Key not found");
        }

        /// retrieve the value at the specified index in the JSON list. 
        /// if the JSON object is not a list or the index is out of bound, return empty JSON 
        constexpr Json operator[](size_t index) const {
            if (!is_iterable()) {
                return Json::error("JSON object is not iterable");
            }

            size_t i = 0;
            for (auto json : *this) if (i++ == index) {
                return json;
            }
            return Json::error("Index is out of bounds");
        }

        // iteration support
        constexpr Json begin() const {
            return is_iterable()
                ? Json::parse(text.substr(1, text.len() - 1), type)
                : Json::error("JSON object is not iterable");
        }

        constexpr Json end() const { return {}; }
        constexpr bool operator!=(const Json&) const { return bool(*this); }
        constexpr Json& operator++() { *this = next(); return *this; }
        constexpr const Json& operator*() const { return *this; }
        constexpr Json& operator*() { return *this; }

        // structured binding support
        template <std::size_t Index>
        constexpr auto get() const {
            if constexpr (Index == 0) {
                return key;
            } else if constexpr (Index == 1) {
                return is_dictionary_item() ? Json::parse(value, Type::DICTIONARY_ITEM) : Json::error("Invalid JSON type");
            }
        }

        constexpr int to_int_or(int other) const { return type == Type::NUMBER ? text.to_int_or(other) : other; }
        constexpr float to_float_or(float other) const { return type == Type::NUMBER ? text.to_float_or(other) : other; }
        constexpr StringView to_string_or(StringView other) const { 
            return type == Type::STRING ? text.substr(1, text.len() - 2) : other; // remove ""
        }

        constexpr int to_int() const { return to_int_or(0); }
        constexpr float to_float() const { return to_float_or(0); }
        constexpr StringView to_string() const { return to_string_or(""); }

        constexpr StringView error_message() const { return err; }
        constexpr Type get_type() const { return type; }

        constexpr bool is_dictionary() const { return type == Type::DICTIONARY; }
        constexpr bool is_list() const { return type == Type::LIST; }
        constexpr bool is_iterable() const { return is_dictionary() || is_list(); }
        constexpr bool is_dictionary_item() const { return type == Type::DICTIONARY_ITEM; }
        constexpr bool is_number() const { return type == Type::NUMBER; }
        constexpr bool is_string() const { return type == Type::STRING; }
        constexpr bool is_bool() const { return type == Type::BOOL; }
        constexpr bool is_true() const { return text == "true"; }
        constexpr bool is_false() const { return text == "false"; }
        constexpr bool is_null() const { return text == "null"; }

    private:
        constexpr Json(
            StringView text,
            StringView err,
            StringView next_text,
            StringView key,
            StringView value,
            Type type,
            Type parent_type
        ) : text(text), err(err), next_text(next_text), key(key), value(value), type(type), parent_type(parent_type) {}

        static constexpr Json parse(StringView text, Type parent_type);
        static constexpr Json error(StringView msg) { return Json({}, msg, {}, {}, {}, Type::UNKNOWN, Type::UNKNOWN); }

        StringView text;
        StringView err = {};
        StringView next_text = {};
        StringView key = {};
        StringView value = {};
        Type type = Type::UNKNOWN;
        Type parent_type = Type::UNKNOWN;
    };

    inline constexpr Json Json::parse(StringView text, Type parent_type) {
        StringView err = {};
        StringView next_text = {};
        StringView key = {};
        StringView value = {};
        Type type = Type::UNKNOWN;

        size_t start_pos = 0, end_pos = 0;
        constexpr auto is_number_ = [] (char ch) { return ch >= '0' && ch <= '9'; };

        // find start position
        for (size_t &i = start_pos; i < text.len(); ++i) {
            if (text[i] == ' ' || text[i] == '\n' || text[i] == '\t' || text[i] == '\r') {
                continue;
            }
            
            const auto text_4 = text.substr(start_pos, start_pos + 4);
            const auto text_5 = text.substr(start_pos, start_pos + 5);

            if (text_4 == "null") {
                type = Type::NULL_;
                end_pos = start_pos + 4;
                break;
            }
            if (text_4 == "true") {
                type = Type::BOOL;
                end_pos = start_pos + 4;
                break;
            }
            if (text_5 == "false") {
                type = Type::BOOL;
                end_pos = start_pos + 5;
                break;
            }
            if (text[i] == '{') {
                type = Type::DICTIONARY;
                end_pos = start_pos + 1;
                break;
            }
            if (text[i] == '[') {
                type = Type::LIST;
                end_pos = start_pos + 1;
                break;
            }
            if (text[i] == '\"') {
                type = Type::STRING;
                end_pos = start_pos + 1;
                break;
            }

            if (is_number_(text[i]) || (text[i] == '-' && is_number_(text[i + 1]))) {
                type = Type::NUMBER;
                end_pos = start_pos + 1;
                break;
            }

            err = "Invalid JSON";
            return Json::error("Invalid JSON object");
        }

        // find end position
        if (type == Type::DICTIONARY) {
            for (size_t &i = end_pos, cnt = 0; i < text.len(); ++i) {
                if (text[i] == '}') {
                    if (cnt == 0) {
                        ++i;
                        break;
                    } else {
                        --cnt;
                    }
                }

                if (text[i] == '{') {
                    ++cnt;
                }
            }
        }

        else if (type == Type::LIST) {
            for (size_t &i = end_pos, cnt = 0; i < text.len(); ++i) {
                if (text[i] == ']') {
                    if (cnt == 0) {
                        ++i;
                        break;
                    } else {
                        --cnt;
                    }
                }

                if (text[i] == '[') {
                    ++cnt;
                }
            }
        }

        else if (type == Type::STRING) {
            for (size_t &i = end_pos; i < text.len(); ++i) {
                if (text[i] == '\"') {
                    ++i;
                    break;
                }
            }
        }

        else if (type == Type::NUMBER) {
            bool found_e = false;
            for (size_t &i = end_pos; i < text.len(); ++i) {
                if (!is_number_(text[i])) {
                    if (!found_e && (text[i] == 'e' || text[i] == 'E')) {
                        found_e = true;
                        if (text[i + 1] == '-') {
                            ++i;
                        }
                        continue;
                    }
                    if (text[i] == '.') {
                        continue;
                    }
                    break;
                }
            }
        }

        // find mismatch
        for (size_t i = end_pos; i < text.len(); ++i) {
            if (text[i] == ' ' || text[i] == '\n' || text[i] == '\t' || text[i] == '\r') {
                continue;
            }

            if (text[i] == ',') {
                next_text = text.substr(i + 1, text.len() - (i + 1));
                break;
            }
            
            if (parent_type == Type::DICTIONARY) {
                if (type == Type::STRING && text[i] == ':') {
                    type = Type::DICTIONARY_ITEM;
                    key = text.substr(start_pos + 1, end_pos - start_pos - 2); // remove ""
                    value = Json::parse(text.substr(i + 1, text.len() - (i + 1)), Type::DICTIONARY).text;
                    end_pos = value.end() - text.begin();
                    i = end_pos - 1;
                    continue;
                } 
                
                if (type == Type::DICTIONARY_ITEM && text[i] == '}') {
                    next_text = text.substr(i + 1, text.len() - (i + 1));
                    break;
                }

                err = "Missing or mismatched braces";
                break;
            }

            if (parent_type == Type::LIST) {
                if (text[i] == ']') {
                    next_text = text.substr(i + 1, text.len() - (i + 1));
                    break;
                }

                err = "Missing or mismatched brackets";
                break;
            }

            err = "Missing or mismatched comma separation";
            break;
        }

        return { text.substr(start_pos, end_pos - start_pos), err, next_text, key, value, type, parent_type };
    }
}

// Specialize std::tuple_size for etl::Json
template <>
struct std::tuple_size<Project::etl::Json> : Project::etl::integral_constant<size_t, 2> {};

// Specialize std::tuple_element for etl::Json
template <size_t Index>
struct std::tuple_element<Index, Project::etl::Json> {
    using type = decltype(Project::etl::declval<Project::etl::Json>().get<Index>());
};

#endif //ETL_JSON_H
