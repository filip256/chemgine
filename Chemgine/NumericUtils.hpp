#pragma once

#include <limits>
#include <cmath>

namespace Utils
{
	template<typename T>
	bool equal(const T x, const T y, const T epsilon = std::numeric_limits<T>::epsilon());
	template<typename T1, typename T2>
	bool equal(const T1 x, const T2 y, const T1 epsilon = std::numeric_limits<T1>::epsilon());
}

template<typename T>
bool Utils::equal(const T x, const T y, const T epsilon)
{
	// bounded relative epsilon comparison
	// https://realtimecollisiondetection.net/blog/?p=89
	return std::abs(x - y) <= epsilon * std::max(static_cast<T>(1.0), std::max(std::abs(x), std::abs(y)));
}

template<typename T1, typename T2>
bool Utils::equal(const T1 x, const T2 y, const T1 epsilon)
{
	return Utils::equal(x, static_cast<T1>(y), epsilon);
}
