#pragma once

#include <set>
#include <unordered_set>

// Base class for types which encapsulate both the key and the values corresponding to that key, allowing efficient
// storage inside set-like containers by implementing the required heterogeneous lookup callables (Hash, Equal, Less).
template <typename KeyT>
class KeyValueObject
{
protected:
    KeyT key;

    KeyValueObject(const KeyT key) noexcept :
        key(key)
    {}

public:
    virtual ~KeyValueObject() = default;

private:
    struct Hash
    {
        using is_transparent = void;

        size_t operator()(const KeyValueObject& obj) const noexcept { return std::hash<KeyT>{}(obj.key); }

        size_t operator()(const KeyT key) const noexcept { return std::hash<KeyT>{}(key); }
    };

    struct Equal
    {
        using is_transparent = void;

        bool operator()(const KeyValueObject& lhs, const KeyValueObject& rhs) const noexcept
        {
            return lhs.key == rhs.key;
        }

        bool operator()(const KeyValueObject& lhs, const KeyT rhs) const noexcept { return lhs.key == rhs; }

        bool operator()(const KeyT lhs, const KeyValueObject& rhs) const noexcept { return lhs == rhs.key; }
    };

    struct Less
    {
        using is_transparent = void;

        bool operator()(const KeyValueObject& lhs, const KeyValueObject& rhs) const { return lhs.key < rhs.key; }

        bool operator()(const KeyValueObject& lhs, const KeyT rhs) const { return lhs.key < rhs; }

        bool operator()(const KeyT lhs, const KeyValueObject& rhs) const { return lhs < rhs.key; }
    };

protected:
    template <typename T>
    using UnorderedSet = std::unordered_set<T, Hash, Equal>;
    template <typename T>
    using Set = std::set<T, Less>;
};
