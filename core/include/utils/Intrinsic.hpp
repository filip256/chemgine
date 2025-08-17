#pragma once

#include <cstdint>

#ifdef _MSC_VER
    #include <intrin.h>
#endif

namespace utils::builtin
{
    inline uint8_t bitScanReverse32(const uint32_t x)
    {
#if defined(_MSC_VER)
        unsigned long index;
        _BitScanReverse(&index, x);
        return static_cast<uint8_t>(index);
#else
        return static_cast<uint8_t>(31 - __builtin_clz(x));
#endif
    }

    inline uint8_t bitScanReverse64(const uint64_t x)
    {
#if defined(_MSC_VER)
        unsigned long index;
#if defined(_M_X64) || defined(_M_ARM64)
        _BitScanReverse64(&index, x);
        return static_cast<uint8_t>(index);
#else
        // Emulate _BitScanReverse64 using two _BitScanReverse32's.
        const uint32_t high = static_cast<uint32_t>(x >> 32);
        if (high != 0)
        {
            _BitScanReverse(&index, high);
            return static_cast<uint8_t>(index + 32);
        }

        _BitScanReverse(&index, static_cast<uint32_t>(x));
        return static_cast<uint8_t>(index);
#endif
#else
        return static_cast<uint8_t>(63 - __builtin_clzll(x));
#endif
    }
}
