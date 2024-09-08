#pragma once

#include "Utils.hpp"
#include "StringUtils.hpp"

#include <vector>
#include <string>
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

			if (str == "t" || str == "TRUE")
				return std::optional(true);
			if (str == "f" || str == "FALSE")
				return std::optional(false);

			return std::nullopt;
		}
	};

	template <typename T>
	class Parser<std::vector<T>>
	{
	public:
		static std::optional<std::vector<T>> parse(const std::string& str, const bool ignoreEmpty = true)
		{
			if (str.starts_with('{') && str.ends_with('}'))
				return parse(str.substr(1, str.size() - 2), ignoreEmpty);

			std::vector<T> result;
			size_t lastSep = -1;
			for (size_t i = 0; i < str.size(); ++i)
				if (str[i] == ',')
				{
					if (ignoreEmpty == false || i - lastSep - 1 > 0)
					{
						auto r = Def::parse<T>(str.substr(lastSep + 1, i - lastSep - 1));
						if (r.has_value())
							result.emplace_back(std::move(*r));
						else
							return std::nullopt;
					}
					lastSep = i;
				}

			if (ignoreEmpty == false || lastSep + 1 < str.size())
			{
				auto r = Def::parse<T>(str.substr(lastSep + 1));
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
	public:
		static std::optional<std::pair<T1, T2>> parse(const std::string& str, const char sep = '@')
		{
			const auto& pairStr = Utils::split(str, sep, true);
			if (pairStr.size() != 2)
				return std::nullopt;

			const auto val1 = Def::parse<T1>(pairStr.front());
			if (val1.has_value() == false)
				return std::nullopt;

			const auto val2 = Def::parse<T2>(pairStr.back());
			if (val2.has_value() == false)
				return std::nullopt;

			return std::optional<std::pair<T1, T2>>(std::make_pair(*val1, *val2));
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
