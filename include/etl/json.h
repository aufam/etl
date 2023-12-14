#ifndef ETL_JSON_H
#define ETL_JSON_H

#include "etl/string_view.h"

namespace Project::etl {

    class Json {
    public:
        enum class Type { UNKNOWN, DICTIONARY, DICTIONARY_ITEM, LIST, STRING, NUMBER, BOOL, NULL_, };

        /// empty constructor
        constexpr Json() : text() {}

        /// default constructor
        explicit Json(StringView text) : text(text) { parse(); }

        /// return false is type is unknown
        explicit operator bool() { return type != Type::UNKNOWN; }

        /// retrieve next json object
        Json next() const { return Json(next_text, parent_type); }

        /// retrieve the value associated with a given key in the JSON dictionary. 
        /// if the JSON object is not a dictionary or the key is not found, return empty JSON 
        Json operator[](StringView key) const {
            if (type != Type::DICTIONARY) {
                return {};
            }

            for (auto json = Json(text.substr(1, text.len() - 1), type); json; json = json.next()) {
                if (json.key == key)
                    return Json(json.value, Type::DICTIONARY_ITEM);
            }
            return {};
        }

        /// retrieve the value at the specified index in the JSON list. 
        /// if the JSON object is not a list or the index is out of bound, return empty JSON 
        Json operator[](size_t index) const {
            if (type != Type::LIST) {
                return {};
            }

            size_t i = 0;
            for (auto json = Json(text.substr(1, text.len() - 1), type); json; json = json.next(), ++i) {
                if (i == index)
                    return json;
            }
            return {};
        }

        int to_int_or(int other) const {
            if (type != Type::NUMBER) {
                return other;
            }
            return text.to_int_or(other);
        }


        float to_float_or(float other) const {
            if (type != Type::NUMBER) {
                return other;
            }
            return text.to_float_or(other);
        }

        StringView to_string_or(StringView other) const {
            if (type != Type::STRING) {
                return other;
            }
            return text.substr(1, text.len() - 2); // remove ""
        }

        int to_int() const { return to_int_or(0); }
        float to_float() const { return to_float_or(0); }
        StringView to_string() const { return to_string_or(""); }

        bool is_true() const { return text == "true"; }
        bool is_false() const { return text == "false"; }
        bool is_null() const { return text == "null"; }

        StringView error_message() { return err; }

    private:
        Json(StringView text, Type parent_type) : text(text), parent_type(parent_type) {
            parse();
        }

        void parse();

        StringView text;
        StringView err = {};
        StringView next_text = {};
        StringView key = {};
        StringView value = {};
        Type type = Type::UNKNOWN;
        Type parent_type = Type::UNKNOWN;
    };

    inline void Json::parse() {
        size_t start_pos = 0, end_pos = 0;

        // find start position
        for (size_t &i = start_pos; i < text.len(); ++i) {
            if (text[i] == ' ' or text[i] == '\n' or text[i] == '\t' or text[i] == '\r') {
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

            auto is_number_ = [] (char ch) { return ch >= '0' and ch <= '9'; };
            if (is_number_(text[i]) or (text[i] == '-' && is_number_(text[i + 1]))) {
                type = Type::NUMBER;
                end_pos = start_pos + 1;
                break;
            }

            err = "Invalid JSON";
            return;
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
            auto is_number_ = [] (char ch) { return (ch >= '0' and ch <= '9'); };
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
            if (text[i] == ' ' or text[i] == '\n' or text[i] == '\t' or text[i] == '\r') {
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
                    value = Json(text.substr(i + 1, text.len() - (i + 1)), Type::DICTIONARY).text;
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

        text = text.substr(start_pos, end_pos - start_pos);
    }
}

#endif //ETL_JSON_H
