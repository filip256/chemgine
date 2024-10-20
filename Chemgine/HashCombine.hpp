#pragma once
		
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