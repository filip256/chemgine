#include "Maths.hpp"

#include <algorithm>
#include <numbers>

constexpr uint32_t Maths::gcd(uint32_t a, uint32_t b)
{
	//Stein algorithm
	if (a == 0)
		return b;
	if (b == 0)
		return a;

	uint8_t k = 0;
	while (((a | b) & 1) == 0)
	{
		a >>= 1;
		b >>= 1;
		++k;
	}

	while ((a & 1) == 0)
		a >>= 1;

	do
	{
		while ((b & 1) == 0)
			b >>= 1;

		if (a > b)
			std::swap(a, b);

		b -= a;
	} while (b != 0);

	return a << k;
}

constexpr uint32_t Maths::lcm(uint32_t a, uint32_t b)
{
	return a / gcd(a, b) * b;
}

bool Maths::isInteger(const float_n v, const float_n precision)
{
	return std::abs(v - static_cast<int>(v)) < precision;
}

uint32_t Maths::integerCoefficient(float_n v)
{
	// find p, where p * v = int
	if (isInteger(v, 0.05f))
		return 1;

	v = v - static_cast<int>(v);
	float_n p = 1;
	do
	{
		v = 1 / (v - static_cast<int>(v));
		p *= v;
	} 
	while (isInteger(v, 0.05f) == false);
	return static_cast<uint32_t>(p);
}

uint32_t Maths::integerCoefficient(const std::vector<float_n>& list)
{
	if (list.empty())
		return 1;

	uint32_t r = integerCoefficient(list[0]);
	for (size_t i = 1; i < list.size(); ++i)
	{
		r = lcm(r, integerCoefficient(list[i]));
	}
	return r;
}

uint64_t Maths::combinations(uint64_t n, uint64_t k)
{
	if (k > n) 
		return 0;

	if (k * 2 > n) 
		k = n - k;

	if (k == 0) 
		return 1;

	uint64_t result = n;
	for (int i = 2; i <= k; ++i) 
	{
		result *= (n - i + 1);
		result /= i;
	}
	return result;
}

float_n Maths::sqaredDistance(const float_n aX, const float_n aY, const float_n bX, const float_n bY)
{
	return (aX - bX) * (aX - bX) + (aY - bY) * (aY - bY);
}

float_n Maths::sqaredDistance(
	const float_n pX, const float_n pY,
	const float_n rLeft, const float_n rTop, const float_n rRight, const float_n rBottom)
{
	const float_n dx = std::max(std::max(rLeft - pX, pX - rRight), 0.0f);
	const float_n dy = std::max(std::max(rTop - pY, pY - rBottom), 0.0f);
	return dx * dx + dy * dy;
}

std::pair<float_n, float_n> Maths::getSlopeAndIntercept(const float_n aX, const float_n aY, const float_n bX, const float_n bY)
{
	const float_n slope = (bY - aY) / (bX - aX);
	return std::make_pair(slope, aY - slope * aX);
}
