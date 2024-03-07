#ifndef ETL_MAP_H
#define ETL_MAP_H

#include "etl/vector.h"

namespace Project::etl {

    /// collection of key-value pairs, keys are unique
    template <typename K, typename V, typename A = etl::Allocator<Pair<K, V>>>
    class Map : public Vector<Pair<K, V>, A> {
    public:
        typedef K Key;
        typedef V Value;

        using Vector<Pair<K, V>, A>::Vector;

        /// check if a key is in this map
        bool has(const K& key) const { return static_cast<bool>(find(key)); }

        /// get a value given the key
        /// @warning it will throw error null dereference if key does not exist
        V& get(const K& key) { return *find(key); }

        /// get a value given the key
        /// @warning it will throw error null dereference if key does not exist
        const V& get(const K& key) const { return *find(key); }

        /// get a value given the key. if the key does not exist, append new pair and return default constructed V
        template <typename KK>
        V& operator[](KK&& key) {
            auto res = find(key);
            if (res) return *res;
            append_pair(etl::forward<KK>(key), Value{});
            return this->back().y;
        }

        /// get a value given the key. if the key does not exist, return default constructed V
        const V& operator[](const K& key) const {
            auto res = find(key);
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
    
        V* find(const K& key) {
            V* res = nullptr;
            for (auto &[x, y]: *this) if (x == key) res = &y;
            return res;
        }

        const V* find(const K& key) const {
            const V* res = nullptr;
            for (auto &[x, y]: *this) if (x == key) res = &y;
            return res;
        }

    private:
        template <typename KK, typename VV>
        void append_pair(KK&& key, VV&& value) {
            auto newCapacity = this->len() + 1;

            if (this->size() < newCapacity && !this->reserve(newCapacity))
                return;
            
            this->append(etl::Pair<K, V>{K(etl::forward<KK>(key)), V(etl::forward<VV>(value))});
        }
    };

    /// create map using variadic function, type is implicitly specified
    template <typename K, typename V, typename A = etl::Allocator<Pair<K, V>>, typename... Ks, typename... Vs,
        typename = enable_if_t<(is_same_v<K, Ks> && ...) && (is_same_v<V, Vs> && ...)>> auto
    map(const Pair<K, V>& item, const Pair<Ks, Vs>&... items) { return Map<K, V, A> { item, items... }; }

    /// create map using variadic function, type is implicitly specified
    template <typename K, typename V, typename A = etl::Allocator<Pair<K, V>>, typename... Ks, typename... Vs,
        typename = enable_if_t<(is_same_v<K, Ks> && ...) && (is_same_v<V, Vs> && ...)>> auto
    map(Pair<K, V>&& item, Pair<Ks, Vs>&&... items) { return Map<K, V, A> { etl::move(item), etl::move(items)... }; }

    /// create map from initializer list, type is explicitly specified
    template <typename K, typename V, typename A = etl::Allocator<Pair<K, V>>> constexpr auto
    map(std::initializer_list<Pair<K,V>>&& items) { return Map<K, V, A>(etl::move(items)); }

    /// create empty map, capacity is 0
    template <typename K, typename V, typename A = etl::Allocator<Pair<K, V>>> constexpr auto
    map() { return Map<K, V, A> {}; }

    /// create empty map, and set the capacity
    template <typename K, typename V, typename A = etl::Allocator<Pair<K, V>>> auto
    map_reserve(size_t capacity) { return Map<K, V, A>(capacity); }

    /// type traits
    template <typename T> struct is_map : false_type {};
    template <typename K, typename V, typename A> struct is_map<Map<K, V, A>> : true_type {};
    template <typename K, typename V, typename A> struct is_map<const Map<K, V, A>> : true_type {};
    template <typename K, typename V, typename A> struct is_map<volatile Map<K, V, A>> : true_type {};
    template <typename K, typename V, typename A> struct is_map<const volatile Map<K, V, A>> : true_type {};
    template <typename T> inline constexpr bool is_map_v = is_map<T>::value;

    template <typename K, typename V, typename A> struct remove_extent<Map<K, V, A>> { typedef Pair<K, V> type; };
    template <typename K, typename V, typename A> struct remove_extent<const Map<K, V, A>> { typedef Pair<K, V> type; };
    template <typename K, typename V, typename A> struct remove_extent<volatile Map<K, V, A>> { typedef Pair<K, V> type; };
    template <typename K, typename V, typename A> struct remove_extent<const volatile Map<K, V, A>> { typedef Pair<K, V> type; };
}

#endif //ETL_MAP_H
