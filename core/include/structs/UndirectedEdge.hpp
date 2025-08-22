#pragma once

#include "utils/Hash.hpp"

template <typename T>
class UndirectedEdge
{
private:
    std::pair<T, T> indices;

public:
    UndirectedEdge(const T idxA, const T idxB) noexcept;

    const std::pair<T, T>& pair() const;

    T getIdxA() const;
    T getIdxB() const;

    bool operator==(const UndirectedEdge& other) const;
};

template <typename T>
UndirectedEdge<T>::UndirectedEdge(const T idxA, const T idxB) noexcept :
    indices(idxA < idxB ? std::pair(idxA, idxB) : std::pair(idxB, idxA))
{}

template <typename T>
const std::pair<T, T>& UndirectedEdge<T>::pair() const
{
    return indices;
}

template <typename T>
T UndirectedEdge<T>::getIdxA() const
{
    return indices.first;
}

template <typename T>
T UndirectedEdge<T>::getIdxB() const
{
    return indices.second;
}

template <typename T>
bool UndirectedEdge<T>::operator==(const UndirectedEdge& other) const
{
    return this->indices == other.indices;
}

//
// Extras
//

template <typename T>
struct std::hash<UndirectedEdge<T>>
{
    size_t operator()(const UndirectedEdge<T>& edge) const
    {
        return std::hash<std::pair<T, T>>()(edge.pair());
    }
};
