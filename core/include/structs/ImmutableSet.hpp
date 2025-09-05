#pragma once

#include "data/def/Printers.hpp"
#include "utils/STL.hpp"

#include <functional>
#include <set>
#include <vector>

/// <summary>
/// Stores a sorted collection of distinct elements as an immutable vector.
/// Good for both equality checking and random access, while ensuring uniqueness.
/// </summary>
template <typename T>
class ImmutableSet
{
private:
    const std::vector<T> content;

public:
    ImmutableSet() = default;
    ImmutableSet(std::vector<T>&& content) noexcept;
    ImmutableSet(std::set<T>&& content) noexcept;
    ImmutableSet(std::unordered_set<T>&& content) noexcept;
    ImmutableSet(std::initializer_list<T> content) noexcept;
    ImmutableSet(const ImmutableSet&) = default;
    ImmutableSet(ImmutableSet&&)      = default;

    static std::vector<T> toSortedSetVector(std::vector<T>&& set);
    static std::vector<T> toSortedSetVector(std::vector<T>&& set, bool (*compare)(const T&, const T&));
    static std::vector<T> toSortedSetVector(std::set<T>&& set);
    static std::vector<T> toSortedSetVector(std::set<T, std::function<bool(const T&, const T&)>>&& set);
    static std::vector<T> toSortedSetVector(std::unordered_set<T>&& set);

    size_t size() const;
    bool   empty() const;

    const std::vector<T>& getContent() const;

    const T& front() const;
    const T& back() const;

    using Iterator = std::vector<T>::const_iterator;
    Iterator begin() const;
    Iterator end() const;

    const T& operator[](const size_t idx) const;

    bool operator==(const ImmutableSet& other) const;
    bool operator!=(const ImmutableSet& other) const;
};

template <typename T>
ImmutableSet<T>::ImmutableSet(std::vector<T>&& content) noexcept :
    content(ImmutableSet::toSortedSetVector(std::move(content)))
{}

template <typename T>
ImmutableSet<T>::ImmutableSet(std::set<T>&& content) noexcept :
    content(ImmutableSet::toSortedSetVector(std::move(content)))
{}

template <typename T>
ImmutableSet<T>::ImmutableSet(std::unordered_set<T>&& content) noexcept :
    content(ImmutableSet::toSortedSetVector(std::move(content)))
{}

template <typename T>
ImmutableSet<T>::ImmutableSet(std::initializer_list<T> content) noexcept :
    content(ImmutableSet::toSortedSetVector(std::set(content)))
{}

template <typename T>
std::vector<T> ImmutableSet<T>::toSortedSetVector(std::vector<T>&& vector)
{
    // TODO: this check fails many times due to const members
    if constexpr (std::assignable_from<T&, T>)  // inplace if possible
    {
        std::sort(vector.begin(), vector.end());
        vector.erase(std::unique(vector.begin(), vector.end()), vector.end());
        return vector;
    }
    else {
        std::set<T> set(vector.begin(), vector.end());
        return ImmutableSet<T>::toSortedSetVector(std::move(set));
    }
}

template <typename T>
std::vector<T> ImmutableSet<T>::toSortedSetVector(std::vector<T>&& vector, bool (*compare)(const T&, const T&))
{
    // TODO: this check fails many times due to const members
    if constexpr (std::assignable_from<T&, T>)  // inplace if possible
    {
        std::sort(vector.begin(), vector.end(), compare);
        vector.erase(std::unique(vector.begin(), vector.end()), vector.end());
        return vector;
    }
    else {
        std::set<T, std::function<bool(const T&, const T&)>> set(vector.begin(), vector.end(), std::function(compare));
        return ImmutableSet<T>::toSortedSetVector(std::move(set));
    }
}

template <typename T>
std::vector<T> ImmutableSet<T>::toSortedSetVector(std::set<T>&& set)
{
    return std::vector<T>(std::make_move_iterator(set.begin()), std::make_move_iterator(set.end()));
}

template <typename T>
std::vector<T> ImmutableSet<T>::toSortedSetVector(std::set<T, std::function<bool(const T&, const T&)>>&& set)
{
    return std::vector<T>(std::make_move_iterator(set.begin()), std::make_move_iterator(set.end()));
}

template <typename T>
std::vector<T> ImmutableSet<T>::toSortedSetVector(std::unordered_set<T>&& set)
{
    std::set<T> result;
    for (auto&& it : set)
        result.emplace(std::move(it));

    return result;
}

template <typename T>
size_t ImmutableSet<T>::size() const
{
    return content.size();
}

template <typename T>
bool ImmutableSet<T>::empty() const
{
    return content.empty();
}

template <typename T>
const std::vector<T>& ImmutableSet<T>::getContent() const
{
    return content;
}

template <typename T>
const T& ImmutableSet<T>::front() const
{
    return content.front();
}

template <typename T>
const T& ImmutableSet<T>::back() const
{
    return content.back();
}

template <typename T>
ImmutableSet<T>::Iterator ImmutableSet<T>::begin() const
{
    return content.cbegin();
}

template <typename T>
ImmutableSet<T>::Iterator ImmutableSet<T>::end() const
{
    return content.cend();
}

template <typename T>
const T& ImmutableSet<T>::operator[](const size_t idx) const
{
    return content[idx];
}

template <typename T>
bool ImmutableSet<T>::operator==(const ImmutableSet& other) const
{
    return this->content == other.content;
}

template <typename T>
bool ImmutableSet<T>::operator!=(const ImmutableSet& other) const
{
    return this->content != other.content;
}

template <typename T>
class def::Printer<ImmutableSet<T>>
{
public:
    static std::string print(const ImmutableSet<T>& object) { return def::print(object.getContent()); }

    static std::string prettyPrint(const ImmutableSet<T>& object) { return def::prettyPrint(object.getContent()); }
};
