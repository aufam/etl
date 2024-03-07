#ifndef ETL_UNORERED_MAP_H
#define ETL_UNORERED_MAP_H

#include "linked_list.h"

namespace Project::etl {

	template <typename K, typename V, typename A = etl::Allocator<Pair<K, V>>>
	class UnorderedMap : public LinkedList<Pair<K, V>, A> {
	public:
        typedef K Key;
        typedef V Value;

		using LinkedList<Pair<K, V>, A>::LinkedList;

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
			this->push_back(etl::Pair<K, V>{etl::forward<KK>(key), Value{}});
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
            return this->pop_at(index);
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
	};

    /// create map using variadic function, type is implicitly specified
    template <typename K, typename V, typename A = etl::Allocator<Pair<K, V>>, typename... Ks, typename... Vs, 
        typename = enable_if_t<(is_same_v<K, Ks> && ...) && (is_same_v<V, Vs> && ...)>> auto
    unordered_map(const Pair<K, V>& item, const Pair<Ks, Vs>&... items) { return UnorderedMap<K, V, A> { item, items... }; }

    /// create map using variadic function, type is implicitly specified
    template <typename K, typename V, typename A = etl::Allocator<Pair<K, V>>, typename... Ks, typename... Vs,
        typename = enable_if_t<(is_same_v<K, Ks> && ...) && (is_same_v<V, Vs> && ...)>> auto
    unordered_map(Pair<K, V>&& item, Pair<Ks, Vs>&&... items) { return UnorderedMap<K, V, A> { etl::move(item), etl::move(items)... }; }

    /// create map from initializer list, type is explicitly specified
    template <typename K, typename V, typename A = etl::Allocator<Pair<K, V>>> constexpr auto
    unordered_map(std::initializer_list<Pair<K,V>>&& items) { return UnorderedMap<K, V, A>(etl::move(items)); }

    /// create empty map, capacity is 0
    template <typename K, typename V, typename A = etl::Allocator<Pair<K, V>>> constexpr auto
    unordered_map() { return UnorderedMap<K, V, A> {}; }

    /// type traits
    template <typename T> struct is_unordered_map : false_type {};
    template <typename K, typename V, typename A> struct is_unordered_map<UnorderedMap<K, V, A>> : true_type {};
    template <typename K, typename V, typename A> struct is_unordered_map<const UnorderedMap<K, V, A>> : true_type {};
    template <typename K, typename V, typename A> struct is_unordered_map<volatile UnorderedMap<K, V, A>> : true_type {};
    template <typename K, typename V, typename A> struct is_unordered_map<const volatile UnorderedMap<K, V, A>> : true_type {};
    template <typename T> inline constexpr bool is_unordered_map_v = is_unordered_map<T>::value;

    template <typename K, typename V, typename A> struct remove_extent<UnorderedMap<K, V, A>> { typedef Pair<K, V> type; };
    template <typename K, typename V, typename A> struct remove_extent<const UnorderedMap<K, V, A>> { typedef Pair<K, V> type; };
    template <typename K, typename V, typename A> struct remove_extent<volatile UnorderedMap<K, V, A>> { typedef Pair<K, V> type; };
    template <typename K, typename V, typename A> struct remove_extent<const volatile UnorderedMap<K, V, A>> { typedef Pair<K, V> type; };
}


#endif
