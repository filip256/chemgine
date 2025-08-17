#pragma once

#include <utility>

namespace utils
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
        if constexpr (sizeof(T1) + sizeof(T2) <= sizeof(size_t))
            return (static_cast<size_t>(pair.first) << sizeof(T2)) | pair.second;
        else
            return utils::hashCombine(pair.first, pair.second);
    }
};
