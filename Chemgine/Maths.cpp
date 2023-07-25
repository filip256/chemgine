#include <algorithm>

#include "Maths.hpp"

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

bool Maths::isInteger(const float v, const float precision)
{
	return std::abs(v - static_cast<int>(v)) < precision;
}

uint32_t Maths::integerCoefficient(float v)
{
	// find p, where p * v = int
	if (isInteger(v, 0.05))
		return 1;

	v = v - static_cast<int>(v);
	float p = 1;
	do
	{
		v = 1 / (v - static_cast<int>(v));
		p *= v;
	} 
	while (isInteger(v, 0.05) == false);
	return p;
}

uint32_t Maths::integerCoefficient(const std::vector<float>& list)
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