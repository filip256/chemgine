#pragma once

#include <cstdint>
#include <vector>

class Maths
{
public:
	static const double Pi;

	static constexpr uint32_t gcd(uint32_t a, uint32_t b);
	static constexpr uint32_t lcm(uint32_t a, uint32_t b);

	static bool isInteger(const float v, const float precision = 1e-10);

	/// <summary>
	/// Returns the smallest integer N such that v * N is an integer.
	/// </summary>
	static uint32_t integerCoefficient(float v);
	/// <summary>
	/// Given a list of floating point numbers it returns the smallest integer N such that
	/// for every element of the list Li, Li * N is an integer.
	/// </summary>
	static uint32_t integerCoefficient(const std::vector<float>& list);

	/// <summary>
	/// Returns the result of C(n, k) aka "n choose k".
	/// </summary>
	static uint64_t combinations(uint64_t n, uint64_t k);

	/// <summary>
	/// Returns the squared distance between the points A and B.
	/// </summary>
	static float sqaredDistance(const float aX, const float aY, const float bX, const float bY);

	/// <summary>
	/// Returns the squared distance between the point P and a straight rectangle R.
	/// </summary>
	static float sqaredDistance(
		const float pX, const float pY,
		const float rLeft, const float rTop, const float rRight, const float rBottom);

	/// <summary>
	/// Converts degrees to angles.
	/// </summary>
	static float toRadians(const float degrees);
};