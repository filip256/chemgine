#pragma once

#include "Utils.hpp"
#include "StringUtils.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <optional>
#include <stdexcept>

namespace Def
{
	template <typename T, typename... Args>
	static std::optional<T> parse(const std::string& str, Args&&... args);
	template <typename T>
	static std::optional<T> parseUnsigned(const std::string& str);

	template <typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
	static std::optional<E> parseEnum(const std::string& str);

	template <typename T>
	static std::optional<T> parseId(const std::string& str);

	template <typename T, typename = void>
	class Parser
	{
		static_assert(std::is_arithmetic_v<T>, "Parser: Unsupported non-arithmetic type");

	public:
		static std::optional<T> parse(const std::string& str)
		{
			const auto r = Def::parse<int64_t>(str);
			return r.has_value() &&
				*r >= std::numeric_limits<T>::min() && *r <= std::numeric_limits<T>::max() ?
				std::optional<T>(*r) :
				std::nullopt;
		}
	};

	template <>
	class Parser<std::string>
	{
	public:
		static std::optional<std::string> parse(const std::string& str)
		{
			return Utils::strip(str);
		}
	};

	template <>
	class Parser<int64_t>
	{
	public:
		static std::optional<int64_t> parse(const std::string& str)
		{
			if (str.empty())
				return std::nullopt;

			try
			{
				return std::optional<int64_t>(std::stoll(str));
			}
			catch (const std::invalid_argument&)
			{
				return std::nullopt;
			}
			catch (const std::out_of_range&)
			{
				return std::nullopt;
			}
		}
	};

	template <>
	class Parser<uint64_t>
	{
	public:
		static std::optional<uint64_t> parse(const std::string& str)
		{
			if (str.empty())
				return std::nullopt;

			try
			{
				return std::optional<int64_t>(std::stoull(str));
			}
			catch (const std::invalid_argument&)
			{
				return std::nullopt;
			}
			catch (const std::out_of_range&)
			{
				return std::nullopt;
			}
		}
	};

	template <>
	class Parser<float>
	{
	public:
		static std::optional<float> parse(const std::string& str)
		{
			if (str.empty())
				return std::nullopt;

			try
			{
				return std::optional<float>(std::stof(str));
			}
			catch (const std::invalid_argument&)
			{
				return std::nullopt;
			}
			catch (const std::out_of_range&)
			{
				return std::nullopt;
			}
		}
	};

	template <>
	class Parser<double>
	{
	public:
		static std::optional<double> parse(const std::string& str)
		{
			if (str.empty())
				return std::nullopt;

			try
			{
				return std::optional<double>(std::stod(str));
			}
			catch (const std::invalid_argument&)
			{
				return std::nullopt;
			}
			catch (const std::out_of_range&)
			{
				return std::nullopt;
			}
		}
	};

	template <>
	class Parser<bool>
	{
	public:
		static std::optional<int64_t> parse(const std::string& str)
		{
			if (str.empty())
				return std::nullopt;

			if (str == "true")
				return std::optional(true);
			if (str == "false")
				return std::optional(false);

			return std::nullopt;
		}
	};

	template <typename T>
	class Parser<std::vector<T>>
	{
	private:
		static std::string removeBrackets(const std::string& str)
		{
			return str.starts_with('{') && str.ends_with('}') ?
				str.substr(1, str.size() - 2) :
				str;
		}

	public:
		static std::optional<std::vector<T>> parse(const std::string& str, const bool ignoreEmpty = true)
		{
			const auto listStr = removeBrackets(str);

			std::vector<T> result;
			size_t lastSep = -1;
			size_t ignoreSections = 0;

			for (size_t i = 0; i < listStr.size(); ++i)
			{
				if (listStr[i] == '{')
					++ignoreSections;
				else if (listStr[i] == '}' && ignoreSections > 0)
					--ignoreSections;

				if (ignoreSections != 0)
					continue;

				if (listStr[i] != ',')
					continue;

				if (ignoreEmpty == false || i - lastSep - 1 > 0)
				{
					auto r = Def::parse<T>(listStr.substr(lastSep + 1, i - lastSep - 1));
					if (r.has_value())
						result.emplace_back(std::move(*r));
					else
						return std::nullopt;
				}
				lastSep = i;
			}

			if (ignoreEmpty == false || lastSep + 1 < listStr.size())
			{
				auto r = Def::parse<T>(listStr.substr(lastSep + 1));
				if (r.has_value())
					result.emplace_back(std::move(*r));
				else
					return std::nullopt;
			}
			return result;
		}
	};

	template <typename T1, typename T2>
	class Parser<std::pair<T1, T2>>
	{
	private:
		static std::string removeBrackets(const std::string& str)
		{
			return str.starts_with('{') && str.ends_with('}') ?
				str.substr(1, str.size() - 2) :
				str;
		}

	public:
		static std::optional<std::pair<T1, T2>> parse(const std::string& str, const char sep = ':')
		{
			const auto& pairStr = Utils::split(removeBrackets(str), sep, '{', '}', true);
			if (pairStr.size() != 2)
				return std::nullopt;

			const auto val1 = Def::parse<T1>(pairStr.front());
			if (not val1.has_value())
				return std::nullopt;

			const auto val2 = Def::parse<T2>(pairStr.back());
			if (not val2.has_value())
				return std::nullopt;

			return std::optional<std::pair<T1, T2>>(std::make_pair(*val1, *val2));
		}
	};

	template <typename T>
	class Parser<std::unordered_map<std::string, T>>
	{
	public:
		static std::optional<std::unordered_map<std::string, T>> parse(const std::string& str)
		{
			auto entries = Def::parse<std::vector<std::pair<std::string, std::string>>>(str);
			if (not entries.has_value())
				return std::nullopt;

			std::unordered_map<std::string, T> result;
			result.reserve(entries->size());
			for (size_t i = 0; i < entries->size(); ++i)
			{
				auto value = Def::parse<T>((*entries)[i].second);
				if (not value.has_value())
					return std::nullopt;

				result.emplace(std::move((*entries)[i].first), *std::move(value));
			}

			return result;
		}
	};
};


template <typename T, typename... Args>
inline std::optional<T> Def::parse(const std::string& str, Args&&... args)
{
	return Def::Parser<T>::parse(str, std::forward<Args>(args)...);
}

template <typename T>
inline std::optional<T> Def::parseUnsigned(const std::string& str)
{
	const auto r = Def::Parser<T>::parse(str);
	return r.has_value() == false || *r < 0 ?
		std::nullopt :
		std::optional(r);
}

template <typename E, typename>
inline std::optional<E> Def::parseEnum(const std::string& str)
{
	const auto val = parse<std::underlying_type_t<E>>(str);
	return val.has_value() ? std::make_optional(static_cast<E>(*val)) : std::nullopt;
}

template <typename T>
static std::optional<T> Def::parseId(const std::string& str)
{
	if (str.empty() || str.front() != '#')
		return std::nullopt;
	
	return parseUnsigned<T>(str.substr(1));
}
