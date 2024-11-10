#pragma once

#include "Precision.hpp"

#include <cstdint>
#include <vector>
#include <type_traits>

class Maths
{
public:
	static constexpr uint32_t gcd(uint32_t a, uint32_t b);
	static constexpr uint32_t lcm(uint32_t a, uint32_t b);

	static bool isInteger(const float_s v, const float_s precision = 1e-10);

	/// <summary>
	/// Returns the smallest integer N such that v * N is an integer.
	/// </summary>
	static uint32_t integerCoefficient(float_s v);
	/// <summary>
	/// Given a list of floating point numbers it returns the smallest integer N such that
	/// for every element of the list Li, Li * N is an integer.
	/// </summary>
	static uint32_t integerCoefficient(const std::vector<float_s>& list);

	/// <summary>
	/// Returns the result of C(n, k) aka "n choose k".
	/// </summary>
	static uint64_t combinations(uint64_t n, uint64_t k);

	/// <summary>
	/// Returns the squared distance between the points A and B.
	/// </summary>
	static float_s sqaredDistance(const float_s aX, const float_s aY, const float_s bX, const float_s bY);
	/// <summary>
	/// Returns the squared distance between the point P and a straight rectangle R.
	/// </summary>
	static float_s sqaredDistance(
		const float_s pX, const float_s pY,
		const float_s rLeft, const float_s rTop, const float_s rRight, const float_s rBottom);

	/// <summary>
	/// Returns the <slope, intercept> pair of the linear function between the points A and B
	/// </summary>
	static std::pair<float_s, float_s> getSlopeAndIntercept(const float_s aX, const float_s aY, const float_s bX, const float_s bY);

	/// <summary>
	/// Returns the int-casted log2 of a value.
	/// Useful for finding the number of used bits by the value.
	/// </summary>
	static constexpr std::uint8_t ilog2(const int64_t v)
	{
		return v ? 1 + ilog2(v >> 1) : -1;
	}

	template <typename T, typename = std::enable_if<std::is_arithmetic_v<T>>>
	static constexpr T clamp(const T value, const T min, const T max)
	{
		return std::max(std::min(value, max), min);
	}
};
