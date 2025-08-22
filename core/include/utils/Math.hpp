#pragma once

#include "global/Precision.hpp"
#include "utils/Intrinsic.hpp"
#include "utils/Numeric.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

namespace utils
{

constexpr uint32_t gcd(uint32_t a, uint32_t b);
constexpr uint32_t lcm(uint32_t a, uint32_t b);

bool isInteger(const float_s v, const float_s precision = 1e-10);

/// <summary>
/// Returns the smallest integer N such that v * N is an integer.
/// </summary>
uint32_t integerCoefficient(float_s v);
/// <summary>
/// Given a list of floating point numbers it returns the smallest integer N such that
/// for every element of the list Li, Li * N is an integer.
/// </summary>
uint32_t integerCoefficient(const std::vector<float_s>& list);

/// <summary>
/// Returns the result of C(n, k) aka "n choose k".
/// </summary>
uint64_t combinations(const uint64_t n, uint64_t k);

/// <summary>
/// Returns the absolute of the given value as an unsigned or float.
/// </summary>
template <typename T>
constexpr utils::float_or_unsigned_t<T> abs(const T v);

/// <summary>
/// Returns the squared Euclidian distance between two points.
/// </summary>
template <typename A, typename B = A>
auto squaredDistance(const A aX, const A aY, const B bX, const B bY);
/// <summary>
/// Returns the squared Euclidian distance between the point P and a straight rectangle R.
/// </summary>
float_s squaredDistance(
    const float_s pX,
    const float_s pY,
    const float_s rLeft,
    const float_s rTop,
    const float_s rRight,
    const float_s rBottom);

/// <summary>
/// Returns absolute Chebyshev distance between two points.
/// </summary>
template <typename A, typename B = A>
auto chebyshevDistance(const A aX, const A aY, const B bX, const B bY);

template <typename A, typename B = A>
auto dotProduct(const A aX, const A aY, const B bX, const B bY);
template <typename A, typename B = A>
auto crossProduct(const A aX, const A aY, const B bX, const B bY);

/// <summary>
/// Returns the <slope, intercept> pair of the linear function between the points A and B
/// </summary>
std::pair<float_s, float_s>
getSlopeAndIntercept(const float_s aX, const float_s aY, const float_s bX, const float_s bY);

/// <summary>
/// Returns the integral log2 of a value or 255 if the value is 0.
/// Useful for finding the number of used bits by the value.
/// </summary>
template <typename T>
uint8_t ilog2(const T x);
/// <summary>
/// Returns the integral square root of a value.
/// </summary>
template <typename T>
T isqrt(const T x);

template <typename A, typename B = A>
constexpr auto absoluteDifference(const A a, const B b);

template <typename InT, typename OutT = InT>
class LinearQuantization
{
    static_assert(std::is_arithmetic_v<InT>, "LinearQuantization: InT must be an arithmetic type.");
    static_assert(
        std::is_arithmetic_v<OutT>, "LinearQuantization: OutT must be an arithmetic type.");

private:
    float_h scale, shift;
    OutT    outMin, outMax;

public:
    constexpr LinearQuantization(
        const InT inLow, const InT inHigh, const OutT outLow, const OutT outHigh) noexcept;

    constexpr OutT operator()(const InT value) const;
    constexpr OutT operator[](const InT value) const;
};

};  // namespace utils

template <typename T>
uint8_t utils::ilog2(const T x)
{
    static_assert(std::is_unsigned_v<T>, "ilog2(): T must be an unsigned integer type.");

    // We need to check if x != 0 anyway, the indirection can be put to better use if we also
    // provide a small LUT.
    static constexpr std::array<uint8_t, 24> lut = {static_cast<uint8_t>(-1),
                                                    0,
                                                    1,
                                                    1,
                                                    2,
                                                    2,
                                                    2,
                                                    2,
                                                    3,
                                                    3,
                                                    3,
                                                    3,
                                                    3,
                                                    3,
                                                    3,
                                                    3,
                                                    4,
                                                    4,
                                                    4,
                                                    4,
                                                    4,
                                                    4,
                                                    4,
                                                    4};
    if (x < lut.size())
        return lut[x];

    // There are also some efficient non-intrinsic variants:
    // https://stackoverflow.com/questions/11376288/fast-computing-of-log2-for-64-bit-integers
    if constexpr (std::is_same_v<T, uint64_t>)
        return builtin::bitScanReverse64(x);
    else
        return builtin::bitScanReverse32(x);
}

template <typename T>
T utils::isqrt(const T x)
{
    static_assert(std::is_unsigned_v<T>, "isqrt(): T must be an unsigned integer type.");
    return static_cast<uint8_t>(std::sqrt(x));

    // std::sqrt() is likely to be the fastest, but there are quite a few efficient approximative
    // methods:
    // https://stackoverflow.com/questions/4930307/fastest-way-to-get-the-integer-part-of-sqrtn
    // including:
    /*
    // We need to check if x != 0 anyway, the indirection can be put to better use if we

     * * also
    provide a small LUT. static constexpr std::array<uint8_t, 24> lut = { 0, 1, 1, 1,
     * 2,
     * 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4 }; if (x < lut.size())
     * return
     * lut[x];

    // Initial guess: 2 ^ (ilog2(x) / 2)
    T res = T(1) << (ilog2(x)
     * / 2);

    //
     * Newton-Raphson iterations
    res = (res + x / res) >> 1; // x >= 9
 res
     * = (res + x / res)
     * >> 1; // x >= 48
    if constexpr(sizeof(T) >= 2)
        res = (res
     * + x / res) >> 1; // x >=
     * 3712
    if constexpr (sizeof(T) >= 4)
    {
        res =
     * (res + x / res) >> 1; // x >=
     * 16184476
        res = (res + x / res) >> 1; // x >=
     * 4294966935
    }
    if constexpr
     * (sizeof(T) >= 8)
        res = (res + x / res) >> 1;
     * // x >= 18446744073709512015

    return
     * res - (res * res > x);
    */
}

template <typename T>
constexpr utils::float_or_unsigned_t<T> utils::abs(const T v)
{
    return utils::float_or_unsigned_t<T>(std::abs(v));
}

template <typename A, typename B>
auto utils::squaredDistance(const A aX, const A aY, const B bX, const B bY)
{
    static_assert(
        std::is_arithmetic_v<A> && std::is_arithmetic_v<B>,
        "squaredDistance(): Input types must be arithmetic types.");
    const auto dX  = aX - bX;
    const auto dY  = aY - bY;
    const auto sqX = dX * dX;
    const auto sqY = dY * dY;

    // If possible ensure unsigned output type.
    using SqT = std::decay_t<decltype(sqX)>;
    if constexpr (std::is_integral_v<SqT>) {
        using OutT = std::make_unsigned_t<SqT>;
        return static_cast<OutT>(sqX) + static_cast<OutT>(sqY);
    }
    else
        return sqX + sqY;
}

template <typename A, typename B>
auto utils::chebyshevDistance(const A aX, const A aY, const B bX, const B bY)
{
    static_assert(
        std::is_arithmetic_v<A> && std::is_arithmetic_v<B>,
        "chebyshevDistance(): Input types must be arithmetic types.");
    return std::max(absoluteDifference(aX, bX), absoluteDifference(aY, bY));
}

template <typename A, typename B>
auto utils::dotProduct(const A aX, const A aY, const B bX, const B bY)
{
    static_assert(
        std::is_arithmetic_v<A> && std::is_arithmetic_v<B>,
        "dotProduct(): Input types must be arithmetic types.");
    return aX * bX + aY * bY;
}

template <typename A, typename B>
auto utils::crossProduct(const A aX, const A aY, const B bX, const B bY)
{
    static_assert(
        std::is_arithmetic_v<A> && std::is_arithmetic_v<B>,
        "crossProduct(): Input types must be arithmetic types.");
    return aX * bY - aY * bX;
}

template <typename A, typename B>
constexpr auto utils::absoluteDifference(const A a, const B b)
{
    // std::abs(a - b) would not work on unsigned types.
    static_assert(
        std::is_arithmetic_v<A> && std::is_arithmetic_v<B>,
        "absoluteDifference(): Input types must be arithmetic types.");
    const auto diff = a > b ? a - b : b - a;

    // If possible ensure unsigned output type.
    using OutT = utils::float_or_unsigned_t<std::decay_t<decltype(diff)>>;
    return static_cast<OutT>(diff);
}

template <typename InT, typename OutT>
constexpr utils::LinearQuantization<InT, OutT>::LinearQuantization(
    const InT inLow, const InT inHigh, const OutT outLow, const OutT outHigh) noexcept :
    scale(static_cast<float_h>(outHigh - outLow) / (inHigh - inLow)),
    shift(outLow - inLow * scale),
    outMin(std::min(outLow, outHigh)),
    outMax(std::max(outLow, outHigh))
{}

template <typename InT, typename OutT>
constexpr OutT utils::LinearQuantization<InT, OutT>::operator()(const InT value) const
{
    // Output value is computed in high-precision float and clamped to avoid overflow when casted to
    // OutT.
    const auto outValue = std::clamp(
        value * scale + shift,
        static_cast<float_h>(std::numeric_limits<OutT>::lowest()),
        static_cast<float_h>(std::numeric_limits<OutT>::max()));

    if constexpr (std::is_integral_v<OutT>)
        return round_cast<OutT>(outValue);
    else
        return static_cast<OutT>(outValue);
}

template <typename InT, typename OutT>
constexpr OutT utils::LinearQuantization<InT, OutT>::operator[](const InT value) const
{
    const auto output = this->operator()(value);
    return std::clamp(output, outMin, outMax);
}
