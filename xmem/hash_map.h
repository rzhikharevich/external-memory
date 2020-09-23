#ifndef XMEM_HASH_MAP_H
#define XMEM_HASH_MAP_H


#include <type_traits>
#include <unordered_map>


namespace xmem {


template <
    typename Key,
    typename Value,
    typename Hash = std::hash<Key>,
    typename Eq = std::equal_to<Key>
> class HashMap : public std::unordered_map<Key, Value, Hash, Eq> {
private:
    using Super = std::unordered_map<Key, Value, Hash, Eq>;

public:
    using Super::Super;

    template <typename K>
    const Value* get(const K& key) const {
        // TODO: Do not copy – need either C++20 or truly custom HashMap.
        const Key innerKey = key;
        const auto iter = Super::find(innerKey);

        if (iter != Super::end()) {
            return &iter->second;
        }

        return nullptr;
    }
};


};


#endif
