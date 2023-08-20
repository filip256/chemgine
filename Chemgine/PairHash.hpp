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
};