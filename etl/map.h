#ifndef ETL_MAP_H
#define ETL_MAP_H

#include "etl/vector.h"
#include "etl/utility.h"

namespace Project::etl {

    template <class K, class V>
    struct Map : Vector<Pair<K, V>> {
        typedef Pair<K, V> Type;
        typedef K Key;
        typedef V Value;

        bool hasKey(const Key& key) {
            for (auto& [x, y] : *this) if (x == key) return true;
            return false;
        }

        Value& operator[] (const Key& key) {
            for (auto& [x, y] : *this) if (x == key) return y;
            this->append(Type { key, Value {}});
            return this->back().y;
        }
    };
}

#endif //ETL_MAP_H
