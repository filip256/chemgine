#pragma once

#include <cstdint>
#include <vector>

class Maths
{
public:
	static constexpr uint32_t gcd(uint32_t a, uint32_t b);
	static constexpr uint32_t lcm(uint32_t a, uint32_t b);

	static bool isInteger(const float v, const float precision = 1e-10);

	/// <summary>
	/// Returns the smallest integer N such that v * N is an integer
	/// </summary>
	static uint32_t integerCoefficient(float v);
	/// <summary>
	/// Given a list of floating point numbers it returns the smallest integer N such that
	/// for every element of the list Li, Li * N is an integer.
	/// </summary>
	static uint32_t integerCoefficient(const std::vector<float>& list);
};