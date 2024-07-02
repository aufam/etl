#ifndef CPP_FASTAPI_JSON_DESERIALIZE_H
#define CPP_FASTAPI_JSON_DESERIALIZE_H

#include "etl/json_types.h"
#include "etl/json.h"
#include "etl/result.h"
#include "etl/string_view.h"


namespace Project::etl::json {
    template <typename T>
    constexpr etl::Result<T, const char*> deserialize(const etl::Json& js);

    template <typename T>
    constexpr etl::Result<void, const char*> deserialize(const etl::Json& js, T& res) {
        if (js.error_message()) return etl::Err(js.error_message().data());

        if constexpr (etl::is_same_v<T, std::nullptr_t>) {
            if (!js.is_null()) return etl::Err("JSON is not null");
        }
        else if constexpr (etl::is_same_v<T, bool>) {
            if (!js.is_bool()) return etl::Err("JSON is not bool");
            res = js.is_true();
        }
        else if constexpr (etl::is_integral_v<T>) {
            if (!js.is_number()) return etl::Err("JSON is not a number");
            res = T(js.to_int());
        }
        else if constexpr (etl::is_floating_point_v<T>) {
            if (!js.is_number()) return etl::Err("JSON is not a number");
            res = js.to_float();
        }
        else if constexpr (etl::is_same_v<T, std::string>) {
            if (!js.is_string()) return etl::Err("JSON is not a string");
            auto sv = js.to_string();
            res = std::string(sv.data(), sv.len());
        }
        else if constexpr (etl::is_same_v<T, std::string_view>) {
            if (!js.is_string()) return etl::Err("JSON is not a string");
            auto sv = js.to_string();
            res = std::string_view(sv.data(), sv.len());
        }
        else if constexpr (etl::is_same_v<T, etl::StringView>) {
            if (!js.is_string()) return etl::Err("JSON is not a string");
            res = js.to_string();
        }
        else if constexpr (etl::is_string_v<T>) {
            if (!js.is_string()) return etl::Err("JSON is not a string");
            auto sv = js.to_string();
            res("%.*s", sv.len(), sv.data());
        }
        else if constexpr (etl::is_same_v<T, BasicType>) {
            if (js.is_null()) {
                return etl::Ok();
            } else if (js.is_bool()) {
                res = js.is_true();
            } else if (js.is_number()) {
                res = js.to_float();
            } else if (js.is_string()) {
                auto sv = js.to_string();
                res = std::string(sv.data(), sv.len());
            } else {
                return etl::Err("JSON is not a basic type");
            }
        }
        else if constexpr (etl::is_vector_v<T>) {
            if (!js.is_list()) return etl::Err("JSON is not a list");
            for (auto item : js) {
                deserialize<typename T::value_type>(item).then([&res] (typename T::value_type it) {
                    res.append(etl::move(it));
                });
            }
        }
        else if constexpr (etl::is_linked_list_v<T> || detail::is_std_vector_v<T> || detail::is_std_list_v<T>) {
            if (!js.is_list()) return etl::Err("JSON is not a list");
            for (auto item : js) {
                deserialize<typename T::value_type>(item).then([&res] (typename T::value_type it) {
                    res.push_back(etl::move(it));
                });
            }
        }
        else if constexpr (etl::is_array_v<T> || detail::is_std_array_v<T>) {
            if (!js.is_list()) return etl::Err("JSON is not a list");
            size_t i = 0;
            for (auto item : js) {
                if (i == res.size()) break;
                deserialize<typename T::value_type>(item).then([&] (typename T::value_type it) {
                    res[i++] = etl::move(it);
                });
            }
        }
        else if constexpr (etl::is_map_v<T> || etl::is_unordered_map_v<T> || detail::is_std_map_v<T> || detail::is_std_unordered_map_v<T>) {
            if (!js.is_dictionary()) return etl::Err("JSON is not a map");
            for (auto [key, value] : js) {
                deserialize<typename detail::json_map<T>::value>(value).then([&] (typename detail::json_map<T>::value it) {
                    res[typename detail::json_map<T>::key(key.data(), key.len())] = etl::move(it);
                });
            }
        }
        else if constexpr (etl::is_optional_v<T>) {
            if (js.is_null()) {
                res = etl::none;
            } else if (res) {
                return deserialize<typename T::type>(js, *res);
            } else {
                if constexpr (std::is_default_constructible_v<typename T::type>) {
                    deserialize<typename T::type>(js).then([&res] (typename T::type it) {
                        res = etl::move(it);
                    });
                } else {
                    return etl::Err("Optional type is not default constructible");
                }
            }
        } 
        else if constexpr (detail::is_std_optional_v<T>) {
            if (js.is_null()) {
                res = etl::none;
            } else if (res) {
                return deserialize<typename T::value_type>(js, *res);
            } else {
                if constexpr (std::is_default_constructible_v<typename T::value_type>) {
                    deserialize<typename T::value_type>(js).then([&res] (typename T::value_type it) {
                        res = etl::move(it);
                    });
                } else {
                    return etl::Err("Optional type is not default constructible");
                }
            }
        } 
        else if constexpr (etl::is_ref_v<T>) {
            if constexpr (etl::is_const_v<typename T::type>) {
                return etl::Err("Cannot deserialize const reference");
            } else {
                return deserialize<typename T::type>(js, *res);
            }
        } else {
            return etl::Err("Cannot deserialize the type");
        }
        return etl::Ok();
    }

    template <typename T>
    constexpr etl::Result<T, const char*> deserialize(const etl::Json& js) {
        T res = {};
        return deserialize(js, res).then([&]() { return etl::move(res); });
    }

    template <typename T>
    constexpr etl::Result<T, const char*> deserialize(etl::StringView j) {
        return deserialize<T>(etl::Json::parse(j));
    }

    template <typename T>
    constexpr etl::Result<void, const char*> deserialize(etl::StringView j, T& res) {
        return deserialize<T>(etl::Json::parse(j), res);
    }

    template <typename T>
    constexpr etl::Result<T, const char*> deserialize(const char* j) {
        return deserialize<T>(etl::StringView(j));
    }

    template <typename T>
    constexpr etl::Result<void, const char*> deserialize(const char* j, T& res) {
        return deserialize<T>(etl::StringView(j), res);
    }

    template <typename T>
    constexpr etl::Result<T, const char*> deserialize(std::string_view j) {
        return deserialize<T>(etl::StringView(j.data(), j.size()));
    }

    template <typename T>
    constexpr etl::Result<void, const char*> deserialize(std::string_view j, T& res) {
        return deserialize<T>(etl::StringView(j.data(), j.size()), res);
    }

    template <typename T>
    constexpr etl::Result<T, const char*> deserialize(const std::string& j) {
        return deserialize<T>(etl::StringView(j.data(), j.size()));
    }

    template <typename T>
    constexpr etl::Result<void, const char*> deserialize(const std::string& j, T& res) {
        return deserialize<T>(etl::StringView(j.data(), j.size()), res);
    }
}


namespace Project::etl::detail {
    template <typename T>
    constexpr etl::Result<void, const char*> json_deserialize(const etl::Json& j, const char* key, T& value) {
        auto js = j[key];
        if (js.error_message()) return etl::Err(js.error_message().data());
        return json::deserialize(js, value);
    }

    constexpr etl::Result<void, const char*> json_deserialize_variadic(const etl::Json&) {
        return etl::Ok();
    }

    template <typename T, typename... Ts>
    constexpr etl::Result<void, const char*> json_deserialize_variadic(
        const etl::Json& j, 
        etl::Pair<const char*, const T&> pair, 
        etl::Pair<const char*, const Ts&>... pairs
    ) {
        etl::Result<void, const char*> res = json_deserialize(j, pair.x, const_cast<T&>(pair.y));
        if (res.is_ok()) {
            return json_deserialize_variadic(j, pairs...);
        } else {
            return res;
        }
    }
}
#endif
