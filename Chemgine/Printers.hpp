#pragma once

#include "Utils.hpp"
#include "StringUtils.hpp"
#include "NumericUtils.hpp"
#include "Keywords.hpp"
#include "Linguistics.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <optional>
#include <stdexcept>
#include <format>
#include <ostream>

namespace Def
{
	template <typename T, typename... Args>
	static std::string print(const T& object, Args&&... arg);
	template <typename T, typename... Args>
	static std::string prettyPrint(const T& object, Args&&... arg);

	template <typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
	static std::string printEnum(const E object);

	template <typename T, typename = void>
	class Printer
	{
		static_assert(std::is_arithmetic_v<T>, "Printer: Unsupported type.");

	public:
		static std::string print(const T object)
		{
			return std::to_string(object);
		}
	};

	template <typename T>
	class Printer<T, std::enable_if_t<std::is_floating_point_v<T>>>
	{
	public:
		static std::string print(const T object)
		{
			return 
				Utils::floatEqual(object, std::numeric_limits<T>::lowest()) ? Def::Amounts::Min :
				Utils::floatEqual(object, std::numeric_limits<T>::max()) ? Def::Amounts::Max :
				Linguistics::formatFloatingPoint(std::format("{:.{}f}", object, 5));
		}
	};

	template <>
	class Printer<std::string>
	{
	public:
		static std::string print(const std::string& str)
		{
			return Utils::strip(str);
		}
	};

	template <>
	class Printer<bool>
	{
	public:
		static std::string print(const bool object)
		{
			return object ? "true" : "false";
		}
	};

	template <typename T>
	class Printer<T*>
	{
	public:
		static std::string print(const T* object)
		{
			return Linguistics::toHex(object);
		}
	};

	template <typename T1, typename T2>
	class Printer<std::pair<T1, T2>>
	{
	public:
		static std::string print(const std::pair<T1, T2>& object)
		{
			return '{' + Def::print(object.first) + ':' + Def::print(object.second) + '}';
		}

		static std::string prettyPrint(const std::pair<T1, T2>& object)
		{
			return "{ " + Def::prettyPrint(object.first) + ": " + Def::prettyPrint(object.second) + " }";
		}
	};

	template <typename T>
	class Printer<std::vector<T>>
	{
	public:
		static std::string print(const std::vector<T>& object)
		{
			if (object.size() == 1)
				return Def::print(object.front());

			std::string result = "{";
			for (size_t i = 0; i < object.size(); ++i)
				result += Def::print(object[i]) + ',';

			result.back() = '}';
			return result;
		}

		static std::string prettyPrint(const std::vector<T>& object)
		{
			if (object.size() == 1)
				return Def::prettyPrint(*object.begin());

			std::string result = "{ ";
			for (size_t i = 0; i < object.size(); ++i)
				result += Def::prettyPrint(object[i]) + ", ";

			result[result.size() - 2] = ' ';
			result.back() = '}';
			return result;
		}
	};

	template <typename T>
	class Printer<std::unordered_map<std::string, T>>
	{
	public:
		static std::string print(const std::unordered_map<std::string, T>& object)
		{
			if (object.size() == 1)
			{
				const auto& p = *object.begin();
				return p.first + ':' + Def::print(p.second);
			}

			std::string result = "{";
			for (const auto& p : object)
				result += p.first + ':' + Def::print(p.second) + ',';

			result.back() = '}';
			return result;
		}

		static std::string prettyPrint(const std::unordered_map<std::string, T>& object)
		{
			if (object.size() == 1)
			{
				const auto& p = *object.begin();
				return p.first + ": " + Def::prettyPrint(p.second);
			}

			std::string result = "{ ";
			for (const auto& p : object)
				result += p.first + ": " + Def::prettyPrint(p.second) + ", ";

			result[result.size() - 2] = ' ';
			result.back() = '}';
			return result;
		}
	};

	template <typename T>
	class Printer<std::unordered_set<T>>
	{
	public:
		static std::string print(const std::unordered_set<T>& object)
		{
			if (object.size() == 1)
				return Def::print(*object.begin());

			std::string result = "{";
			for (const auto& i : object)
				result += Def::print(i) + ',';

			result.back() = '}';
			return result;
		}

		static std::string prettyPrint(const std::unordered_set<T>& object)
		{
			if (object.size() == 1)
				return Def::prettyPrint(*object.begin());

			std::string result = "{ ";
			for (const auto& i : object)
				result += Def::prettyPrint(i) + ", ";

			result[result.size() - 2] = ' ';
			result.back() = '}';
			return result;
		}
	};
};



template <typename T, typename... Args>
inline std::string Def::print(const T& object, Args&&... args)
{
	return Def::Printer<T>::print(object, std::forward<Args>(args)...);
}

template <typename, typename = void>
struct has_pretty_print : std::false_type {};

template <typename T>
struct has_pretty_print<T, std::void_t<decltype(&Def::Printer<T>::prettyPrint)>> : std::true_type {};

template <typename T, typename... Args>
inline std::string Def::prettyPrint(const T& object, Args&&... args)
{
	if constexpr (has_pretty_print<T>::value) 
		return Def::Printer<T>::prettyPrint(object, std::forward<Args>(args)...);
	else
		return Def::Printer<T>::print(object, std::forward<Args>(args)...);
}

template <typename E, typename>
static std::string Def::printEnum(const E object)
{
	return std::to_string(static_cast<std::underlying_type_t<E>>(object));
}
