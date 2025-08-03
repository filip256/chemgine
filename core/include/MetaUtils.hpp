#pragma once

#include "NumericUtils.hpp"

#include <ostream>

namespace utils
{
	template <typename...>
	inline constexpr bool always_false = false;

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

	template<typename T>
	concept Streamable = requires(std::ostream & os, T t) { { os << t } -> std::same_as<std::ostream&>; };
	template <typename T>
	constexpr bool is_streamable_v = Streamable<T>;

	template <typename T>
	auto invokeOrForward(T&& obj);

	template<typename T>
	using RefType = std::conditional_t<(sizeof(T) > 8), const T&, const T>;
}

template<typename T>
bool utils::equal(const T x, const T y)
{
	if constexpr (std::is_floating_point_v<T>)
		return utils::floatEqual(x, y);
	else
		return x == y;
}

template<typename T1, typename T2>
bool utils::equal(const T1 x, const T2 y)
{
	return utils::equal(x, static_cast<T1>(y));
}

template <typename T>
auto utils::invokeOrForward(T&& obj)
{
	if constexpr (std::is_invocable_v<std::decay_t<T>>)
		return obj();
	else
		return obj;
}
