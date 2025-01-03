#pragma once

#include "NumericUtils.hpp"

#include <type_traits>

namespace Utils
{
	template<typename T>
	bool equal(const T x, const T y);
	template<typename T1, typename T2>
	bool equal(const T1 x, const T2 y);

	namespace
	{
		template <typename First, typename... Rest>
		struct front
		{
			static_assert(std::is_class_v<First>, "front: First argument must be a type.");

			using type = First;
		};
	}

	// Extracts the first type out of a parameter pack.
	template <typename... Ts>
	using front_t = typename front<Ts...>::type;

	namespace
	{
		template <template <typename...> class, typename...>
		struct is_specialization_of : std::false_type {};
		template <template <typename...> class Template, typename... Args>
		struct is_specialization_of<Template, Template<Args...>> : std::true_type {};
	}

	// Checks is a given type is a specialization of a template.
	template <typename T, template <typename...> class Template>
	constexpr bool is_specialization_of_v = is_specialization_of<Template, T>::value;

	namespace
	{
		template <typename Derived, template <typename...> class Base>
		struct is_derived_from_template
		{
		private:
			static constexpr std::false_type test(...);

			template <typename... Args>
			static constexpr std::true_type test(const Base<Args...>&);

		public:
			static constexpr bool value = decltype(test(std::declval<const Derived&>()))::value;
		};
	}

	// Checks is a given type is derived from a specialization of a template.
	template <typename T, template <typename...> class Template>
	constexpr bool is_derived_from_template_v = is_derived_from_template<T, Template>::value;

	template <typename T>
	inline constexpr bool is_function_pointer_v = std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>>;

	// Uniquifies a type, acting like a "strong" typedef.
	template<typename T, int64_t N>
	struct unique_t : public T
	{
		static_assert(!is_function_pointer_v<T>, "unique_t: T must be a non-function-pointer type, use unique_func_t instead.");
		using T::T;
	};

	// Uniquifies a funtion pointer type, acting like a "strong" typedef.
	template<typename T, int64_t N>
	struct unique_func_t
	{
		static_assert(is_function_pointer_v<T>, "unique_func_t: T must be a function pointer type, use unique_t instead.");

	public:
		using type = T;
		T funcptr;

		constexpr inline unique_func_t(const T funcptr) noexcept : funcptr(funcptr) {}

		template <typename... Args>
		constexpr inline auto operator()(Args&&... args) const -> decltype((*funcptr)(std::forward<Args>(args)...))
		{
			return funcptr(std::forward<Args>(args)...);
		}

		constexpr inline T operator*() const noexcept { return funcptr; }
		constexpr inline T operator->() const noexcept { return funcptr; }
		constexpr inline operator T() const noexcept { return funcptr; }
		constexpr inline bool operator==(const unique_func_t other) const noexcept { return funcptr == other.funcptr; }
		constexpr inline bool operator!=(const unique_func_t other) const noexcept { return funcptr != other.funcptr; }
	};
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
