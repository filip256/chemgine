#pragma once

#include <limits>
#include <cmath>

namespace Utils
{
	template<typename T>
	bool floatEqual(const T x, const T y, const T epsilon = std::numeric_limits<T>::epsilon());
	template<typename T1, typename T2>
	bool floatEqual(const T1 x, const T2 y, const T1 epsilon = std::numeric_limits<T1>::epsilon());

	template<typename T>
	T roundTo(const T x, const uint8_t decimals);

	template<typename T>
	uint8_t getDigitCount(const T x);
}

template<typename T>
bool Utils::floatEqual(const T x, const T y, const T epsilon)
{
	// bounded relative epsilon comparison
	// https://realtimecollisiondetection.net/blog/?p=89
	return std::abs(x - y) <= epsilon * std::max(static_cast<T>(1.0), std::max(std::abs(x), std::abs(y)));
}

template<typename T1, typename T2>
bool Utils::floatEqual(const T1 x, const T2 y, const T1 epsilon)
{
	return Utils::floatEqual(x, static_cast<T1>(y), epsilon);
}

template<typename T>
T Utils::roundTo(const T x, const uint8_t decimals)
{
	const T scale = std::pow(10.0, decimals);
	return std::round(x * scale) / scale;
}

template<typename T>
uint8_t Utils::getDigitCount(const T x)
{
	static_assert(std::is_integral_v<T>, "T must be an integral type");
	return
		x > 0 ? static_cast<uint8_t>(std::log10(x)) + 1 :
		x < 0 ? static_cast<uint8_t>(std::log10(-x)) + 1 :
		1; // log10(0) is undefined
}
