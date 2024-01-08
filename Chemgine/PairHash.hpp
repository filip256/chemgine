#pragma once

#include <utility>

#include "HashCombine.hpp"

class PairHash
{
public:
    template <class T1, class T2>
    size_t operator() (const std::pair<T1, T2>& pair) const 
    {
        size_t hash = 0;
        hashCombine(hash, pair.first);
        hashCombine(hash, pair.second);
        return hash;
    }

    template <class T1, class T2>
    size_t operator() (const T1 first, const T2 second) const
    {
        size_t hash = 0;
        hashCombine(hash, first);
        hashCombine(hash, second);
        return hash;
    }
};