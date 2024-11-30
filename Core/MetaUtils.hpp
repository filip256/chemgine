#pragma once

#include "NumericUtils.hpp"

namespace Utils
{
	template<typename FisrtT, typename... RestT>
	constexpr FisrtT getFirstArg();

	template<typename T>
	bool equal(const T x, const T y);
	template<typename T1, typename T2>
	bool equal(const T1 x, const T2 y);

	template <typename, template <typename> class>
	struct is_specialization_of : std::false_type {};
	template <typename U, template <typename> class Template>
	struct is_specialization_of<Template<U>, Template> : std::true_type {};
	template <typename T, template <typename> class Template>
	constexpr bool is_specialization_of_v = is_specialization_of<T, Template>::value;
}

template<typename FisrtT, typename... RestT>
constexpr FisrtT Utils::getFirstArg()
{
	return FisrtT;
}

template<typename T>
bool Utils::equal(const T x, const T y)
{
	if constexpr (std::is_floating_point_v<T>)
		return Utils::floatEqual(x, y);
	else
		return x == y;
}

template<typename T1, typename T2>
bool Utils::equal(const T1 x, const T2 y)
{
	return Utils::equal(x, static_cast<T1>(y));
}
