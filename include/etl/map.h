#ifndef ETL_MAP_H
#define ETL_MAP_H

#include "etl/vector.h"

namespace Project::etl {

    /// collection of key-value pairs, keys are unique
    template <typename K, typename V>
    class Map : public Vector<Pair<K, V>> {
    public:
        typedef K Key;
        typedef V Value;

        using Vector<Pair<K, V>>::Vector;

        /// check if a key is in this map
        bool has(const K& key) const { return static_cast<bool>(get_value_ptr_(key)); }

        /// get a value given the key
        /// @warning it will throw error null dereference if key does not exist
        V& get(const K& key) { return *get_value_ptr_(key); }

        /// get a value given the key
        /// @warning it will throw error null dereference if key does not exist
        const V& get(const K& key) const { return *get_value_ptr_(key); }

        /// get a value given the key. if the key does not exist, append new pair and return default constructed V
        template <typename KK>
        V& operator[](KK&& key) {
            auto res = get_value_ptr_(key);
            if (res) return *res;
            append_force_(etl::forward<KK>(key), Value{});
            return this->back().y;
        }

        /// get a value given the key. if the key does not exist, return default constructed V
        const V& operator[](const K& key) const {
            auto res = get_value_ptr_(key);
            if (res) return *res;
            static const auto v = Value{};
            return v;
        }
        
        /// remove key-value pair given the key
        bool remove(const K& key) {
            int index = 0;
            for (auto& pair : *this) {
                if (pair.x == key) break;
                ++index;
            }
            return this->remove_at(index);
        }
    
    private:
        V* get_value_ptr_(const K& key) {
            V* res = nullptr;
            for (auto &[x, y]: *this) if (x == key) res = &y;
            return res;
        }

        const V* get_value_ptr_(const K& key) const {
            const V* res = nullptr;
            for (auto &[x, y]: *this) if (x == key) res = &y;
            return res;
        }

        template <typename KK, typename VV>
        void append_force_(KK&& key, VV&& value) {
            auto newCapacity = this->nItems + 1;

            if (this->capacity < newCapacity)
                this->reserve(newCapacity);
            
            new(this->buf + this->nItems) etl::Pair<K, V>{K(etl::forward<KK>(key)), V(etl::forward<VV>(value))};
            ++this->nItems;
        }
    };

    /// create map using variadic function, type is implicitly specified
    template <typename K, typename V, typename... Ks, typename... Vs>
    enable_if_t<(is_same_v<K, Ks> && ...) && (is_same_v<V, Vs> && ...), Map<K, V>>
    map(const Pair<K, V>& item, const Pair<Ks, Vs>&... items) { return Map<K, V> { item, items... }; }

    /// create map from initializer list, type is explicitly specified
    template <typename K, typename V> constexpr auto
    map(std::initializer_list<Pair<K,V>>&& items) { return Map<K, V>(etl::move(items)); }

    /// create empty map, capacity is 0
    template <typename K, typename V> constexpr auto
    map() { return Map<K, V> {}; }

    /// create empty map, and set the capacity
    template <typename K, typename V> Map<K, V>
    map_reserve(size_t capacity) { return Map<K, V>(capacity); }

    /// type traits
    template <typename T> struct is_map : false_type {};
    template <typename K, typename V> struct is_map<Map<K, V>> : true_type {};
    template <typename K, typename V> struct is_map<const Map<K, V>> : true_type {};
    template <typename K, typename V> struct is_map<volatile Map<K, V>> : true_type {};
    template <typename K, typename V> struct is_map<const volatile Map<K, V>> : true_type {};
    template <typename T> inline constexpr bool is_map_v = is_map<T>::value;

    template <typename K, typename V> struct remove_extent<Map<K, V>> { typedef Pair<K, V> type; };
    template <typename K, typename V> struct remove_extent<const Map<K, V>> { typedef Pair<K, V> type; };
    template <typename K, typename V> struct remove_extent<volatile Map<K, V>> { typedef Pair<K, V> type; };
    template <typename K, typename V> struct remove_extent<const volatile Map<K, V>> { typedef Pair<K, V> type; };
}

#endif //ETL_MAP_H
