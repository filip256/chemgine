#pragma once
		
#include <utility>

namespace Utils
{
    template<typename T>
    inline void hashCombineWith(size_t& prev, const T& last) noexcept
    {
        prev ^= std::hash<T>()(last) + 0x9e3779b9 + (prev << 6) + (prev >> 2);
    }

    template<typename T, typename... Args>
    inline void hashCombineWith(size_t& prev, const T& first, const Args&... rest) noexcept
    {
        hashCombineWith(prev, first);
        hashCombineWith(prev, rest...);
    }

    template<typename T, typename... Args>
    inline size_t hashCombine(const T& first, const Args&... rest) noexcept
    {
        size_t h = std::hash<T>()(first);
        hashCombineWith(h, rest...);
        return h;
    }
}

template<class T1, class T2>
struct std::hash<std::pair<T1, T2>>
{
    size_t operator() (const std::pair<T1, T2>& pair) const noexcept
    {
        size_t hash = 0;
        Utils::hashCombine(hash, pair.first, pair.second);
        return hash;
    }
};
