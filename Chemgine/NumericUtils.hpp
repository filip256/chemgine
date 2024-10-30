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
