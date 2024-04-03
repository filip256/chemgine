#pragma once

#include "DynamicAmount.hpp"
#include "Spline.hpp"
#include "Utils.hpp"
#include "Color.hpp"
#include "LabwarePort.hpp"

#include <vector>
#include <string>
#include <optional>
#include <stdexcept>

class DataHelpers
{
public:
	template <typename T>
	static std::optional<T> parse(const std::string& str);
	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	static std::optional<T> parseUnsigned(const std::string& str);
	template <Unit U>
	static std::optional<Amount<U>> parse(const std::string& str);
	template <Unit U>
	static std::optional<Amount<U>> parseUnsigned(const std::string& str);

	template <typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
	static std::optional<E> parseEnum(const std::string& str);

	template <typename T>
	static std::optional<std::vector<T>> parseList(
		const std::string& line,
		const char sep,
		const bool ignoreEmpty = false);

	static std::vector<std::string> parseList(
		const std::string& line,
		const char sep,
		const bool ignoreEmpty = false);

	static std::vector<std::vector<std::string>> parseLists(
		const std::string& line,
		const char outSep,
		const char inSep,
		const bool ignoreEmpty = false);

	template <typename T1, typename T2>
	static std::optional<std::pair<T1,T2>> parsePair(const std::string& str);
	template <Unit U1, Unit U2>
	static std::optional<std::pair<Amount<U1>, Amount<U2>>> parsePair(const std::string& str);

	template <typename T>
	static std::optional<T> parseId(const std::string& str);
};



template <>
inline std::optional<int64_t> DataHelpers::parse<int64_t>(const std::string& str)
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

template <>
inline std::optional<unsigned int> DataHelpers::parse<unsigned int>(const std::string& str)
{
	const auto r = parse<int>(str);

	if (r.has_value() == false || *r < 0)
		return std::nullopt;

	return std::optional<unsigned int>(*r);
}


template <>
inline std::optional<float> DataHelpers::parse<float>(const std::string& str)
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

template <>
inline std::optional<double> DataHelpers::parse<double>(const std::string& str)
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

template <typename T>
inline std::optional<T> DataHelpers::parse(const std::string& str)
{
	const auto r = parse<int64_t>(str);

	if (r.has_value() == false ||
		*r < std::numeric_limits<T>::min() ||
		*r > std::numeric_limits<T>::max())
		return std::nullopt;

	return std::optional<T>(*r);
}

template <>
inline std::optional<uint64_t> DataHelpers::parseUnsigned<uint64_t>(const std::string& str)
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

template <>
inline std::optional<float> DataHelpers::parseUnsigned<float>(const std::string& str)
{
	auto r = parse<float>(str);

	if (r.has_value() == false || *r < 0)
		return std::nullopt;

	return r;
}

template <>
inline std::optional<double> DataHelpers::parseUnsigned<double>(const std::string& str)
{
	auto r = parse<double>(str);

	if (r.has_value() == false || *r < 0)
		return std::nullopt;

	return r;
}

template <typename T, typename>
inline std::optional<T> DataHelpers::parseUnsigned(const std::string& str)
{
	const auto r = parseUnsigned<uint64_t>(str);

	if (r.has_value() == false ||
		*r < std::numeric_limits<T>::min() ||
		*r > std::numeric_limits<T>::max())
		return std::nullopt;

	return std::optional<T>(*r);
}

template<Unit U>
inline std::optional<Amount<U>> DataHelpers::parse(const std::string& str)
{
	const auto pair = parseList(str, '_', true);

	if (pair.size() == 1)
	{
		const auto val = parse<Amount<>::StorageType>(pair.front());
		return val.has_value() ? 
			std::optional(Amount<U>(*val)) :
			std::nullopt;
	}

	if (pair.size() == 2)
	{
		const auto val = parse<Amount<>::StorageType>(pair.front());
		if (val.has_value() == false)
			return std::nullopt;


		const auto unit = DynamicAmount::getUnitFromSymbol(pair.back());
		if (unit.has_value() == false)
			return std::nullopt;

		return DynamicAmount(*val, *unit).to<U>();
	}

	return std::nullopt;
}

template<Unit U>
inline std::optional<Amount<U>> DataHelpers::parseUnsigned(const std::string& str)
{
	const auto r = parseUnsigned<typename Amount<U>::StorageType>(str);
	if (r.has_value())
		return Amount<U>(*r);
	return std::nullopt;
}

template <>
inline std::optional<bool> DataHelpers::parse<bool>(const std::string& str)
{
	if (str.empty())
		return std::nullopt;

	if (str == "1" || str == "T" || str == "t" || str == "TRUE" || str == "true")
		return std::optional(true);
	if (str == "0" || str == "F" || str == "f" || str == "FALSE" || str == "false")
		return std::optional(false);

	return std::nullopt;
}

template <>
inline std::optional<Color> DataHelpers::parse<Color>(const std::string& str)
{
	if (str.empty())
		return std::nullopt;

	const auto rgba = parseList<uint8_t>(str, ':', true);
	if (rgba.has_value() == false || rgba->size() != 4)
		return std::nullopt;

	return Color((*rgba)[0], (*rgba)[1], (*rgba)[2], (*rgba)[3]);
}

template <>
inline std::optional<Spline<float>> DataHelpers::parse<Spline<float>>(const std::string& str)
{
	const auto& pointsStr = parseList(str, ';', true);

	std::vector<std::pair<float, float>> points;
	points.reserve(pointsStr.size());

	for (size_t i = 0; i < pointsStr.size(); ++i)
	{
		const auto p = parsePair<double, double>(pointsStr[i]);
		if (p.has_value() == false)
			return std::nullopt;
		points.emplace_back(Utils::reversePair(*p));
	}

	return points.empty() ? std::nullopt : std::optional<Spline<float>>(Spline(std::move(points)));
}

template <>
inline std::optional<LabwarePort> DataHelpers::parse<LabwarePort>(const std::string& str)
{
	const auto port = DataHelpers::parseList(str, ':', true);

	if (port.size() != 4)
		return std::nullopt;

	const auto type = DataHelpers::parseEnum<PortType>(port[0]);
	const auto x = DataHelpers::parse<unsigned int>(port[1]);
	const auto y = DataHelpers::parse<unsigned int>(port[2]);
	const auto angle = DataHelpers::parseUnsigned<double>(port[3]);

	if (type.has_value() == false || x.has_value() == false || y.has_value() == false || angle.has_value() == false)
		return std::nullopt;

	return LabwarePort(*type, *x, *y, *angle);
}

template <typename E, typename>
inline std::optional<E> DataHelpers::parseEnum(const std::string& str)
{
	const auto val = parse<std::underlying_type_t<E>>(str);
	return val.has_value() ? std::make_optional(static_cast<E>(*val)) : std::nullopt;
}

template <typename T>
std::optional<std::vector<T>> DataHelpers::parseList(
	const std::string& line,
	const char sep,
	const bool ignoreEmpty)
{
	std::vector<T> result;
	size_t lastSep = -1;

	for (size_t i = 0; i < line.size(); ++i)
		if (line[i] == sep)
		{
			if (ignoreEmpty == false || i - lastSep - 1 > 0)
			{
				auto r = parse<T>(line.substr(lastSep + 1, i - lastSep - 1));
				if (r.has_value())
					result.emplace_back(std::move(*r));
				else
					return std::nullopt;
			}
			lastSep = i;
		}
	if (ignoreEmpty == false || lastSep + 1 < line.size())
	{
		auto r = parse<T>(line.substr(lastSep + 1));
		if (r.has_value())
			result.emplace_back(std::move(*r));
		else
			return std::nullopt;
	}
	return result;
}

template <typename T1, typename T2>
inline std::optional<std::pair<T1, T2>> DataHelpers::parsePair(const std::string& str)
{
	const auto& pairStr = parseList(str, '@', true);
	if (pairStr.size() != 2)
		return std::nullopt;

	const auto val1 = parse<T1>(pairStr.front());
	if (val1.has_value() == false)
		return std::nullopt;

	const auto val2 = parse<T2>(pairStr.back());
	if (val2.has_value() == false)
		return std::nullopt;

	return std::optional<std::pair<T1, T2>>(std::make_pair(*val1, *val2));
}

template <Unit U1, Unit U2>
inline std::optional<std::pair<Amount<U1>, Amount<U2>>> DataHelpers::parsePair(const std::string& str)
{
	const auto& pairStr = parseList(str, '@', true);
	if (pairStr.size() != 2)
		return std::nullopt;

	const auto val1 = parse<U1>(pairStr.front());
	if (val1.has_value() == false)
		return std::nullopt;

	const auto val2 = parse<U2>(pairStr.back());
	if (val2.has_value() == false)
		return std::nullopt;

	return std::optional<std::pair<Amount<U1>, Amount<U2>>>(std::make_pair(*val1, *val2));
}

template <typename T>
static std::optional<T> DataHelpers::parseId(const std::string& str)
{
	if (str.empty() || str.front() != '#')
		return std::nullopt;
	
	return parseUnsigned<T>(str.substr(1));
}
