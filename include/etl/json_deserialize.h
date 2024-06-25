#ifndef CPP_FASTAPI_JSON_DESERIALIZE_H
#define CPP_FASTAPI_JSON_DESERIALIZE_H

#include "etl/json_types.h"
#include "etl/json.h"
#include "etl/result.h"


namespace Project::etl::json {
    template <typename T>
    constexpr etl::Result<T, const char*> deserialize(etl::StringView j);

    template <typename T>
    constexpr etl::Result<void, const char*> deserialize(etl::StringView j, T& res) {
        auto js = etl::Json::parse(j);
        if (js.error_message()) return etl::Err(js.error_message().data());

        if constexpr (etl::is_same_v<T, std::nullptr_t>) {
            if (!js.is_null()) return etl::Err("JSON is not null");
        }
        else if constexpr (etl::is_same_v<T, bool>) {
            if (!js.is_bool()) return etl::Err("JSON is not null");
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
                deserialize<etl::remove_extent_t<T>>(item.dump()).then([&res] (etl::remove_extent_t<T> it) {
                    res.append(etl::move(it));
                });
            }
        }
        else if constexpr (etl::is_linked_list_v<T>) {
            if (!js.is_list()) return etl::Err("JSON is not a list");
            for (auto item : js) {
                deserialize<etl::remove_extent_t<T>>(item.dump()).then([&res] (etl::remove_extent_t<T> it) {
                    res.push(etl::move(it));
                });
            }
        }
        else if constexpr (etl::is_map_v<T>) {
            if (!js.is_dictionary()) return etl::Err("JSON is not a map");
            using Pair = etl::remove_extent_t<T>;
            for (auto [key, value] : js) {
                deserialize<decltype(Pair::y)>(value.dump()).then([&] (decltype(Pair::y) it) {
                    res[std::string(key.data(), key.len())] = etl::move(it);
                });
            }
        }
        else if constexpr (etl::is_unordered_map_v<T>) {
            if (!js.is_dictionary()) return etl::Err("JSON is not a map");
            using Pair = etl::remove_extent_t<T>;
            for (auto [key, value] : js) {
                deserialize<decltype(Pair::y)>(value.dump()).then([&] (decltype(Pair::y) it) {
                    res[std::string(key.data(), key.len())] = etl::move(it);
                });
            }
        }
        else if constexpr (etl::is_optional_v<T>) {
            if (js.is_null()) {
                res = etl::none;
            } else if (res) {
                return deserialize<typename T::type>(js.dump(), *res);
            } else {
                if constexpr (std::is_default_constructible_v<typename T::type>) {
                    deserialize<typename T::type>(js.dump()).then([&res] (typename T::type it) {
                        res = etl::move(it);
                    });
                } else {
                    return etl::Err("Optional type is not default constructible");
                }
            }
        } 
        else if constexpr (etl::is_ref_v<T>) {
            return deserialize<typename T::type>(js.dump(), *res);
        }
        return etl::Ok();
    }

    template <typename T>
    constexpr etl::Result<T, const char*> deserialize(etl::StringView j) {
        T res = {};
        return deserialize(j, res).then([&]() {
            return etl::move(res);
        });
    }
}


namespace Project::etl::detail {
    template <typename T>
    constexpr etl::Result<void, const char*> json_deserialize(const etl::Json& j, const char* key, T& value) {
        auto js = j[key];
        if (js.error_message()) return etl::Err(js.error_message().data());
        return json::deserialize(js.dump(), value);
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