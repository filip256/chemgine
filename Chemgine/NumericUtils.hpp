#pragma once

#include <limits>
#include <cmath>

namespace Utils
{
	template<typename T>
	bool equal(const T x, const T y, const T epsilon = std::numeric_limits<T>::epsilon());
}

template<typename T>
bool Utils::equal(const T x, const T y, const T epsilon)
{
	return std::abs(x - y) <= epsilon;
}
