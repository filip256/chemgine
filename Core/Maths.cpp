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

bool Maths::isInteger(const float_s v, const float_s precision)
{
	return std::abs(v - static_cast<int>(v)) < precision;
}

uint32_t Maths::integerCoefficient(float_s v)
{
	// find p, where p * v = int
	if (isInteger(v, 0.05f))
		return 1;

	v = v - static_cast<int>(v);
	float_s p = 1;
	do
	{
		v = 1 / (v - static_cast<int>(v));
		p *= v;
	} 
	while (isInteger(v, 0.05f) == false);
	return static_cast<uint32_t>(p);
}

uint32_t Maths::integerCoefficient(const std::vector<float_s>& list)
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

float_s Maths::sqaredDistance(const float_s aX, const float_s aY, const float_s bX, const float_s bY)
{
	return (aX - bX) * (aX - bX) + (aY - bY) * (aY - bY);
}

float_s Maths::sqaredDistance(
	const float_s pX, const float_s pY,
	const float_s rLeft, const float_s rTop, const float_s rRight, const float_s rBottom)
{
	const float_s dx = std::max(std::max(rLeft - pX, pX - rRight), 0.0f);
	const float_s dy = std::max(std::max(rTop - pY, pY - rBottom), 0.0f);
	return dx * dx + dy * dy;
}

std::pair<float_s, float_s> Maths::getSlopeAndIntercept(const float_s aX, const float_s aY, const float_s bX, const float_s bY)
{
	const float_s slope = (bY - aY) / (bX - aX);
	return std::make_pair(slope, aY - slope * aX);
}
