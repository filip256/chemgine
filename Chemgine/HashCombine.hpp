#pragma once

#include <functional>
		
template <class T>
inline void hashCombine(std::size_t& seed, const T& v)
{
    seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}