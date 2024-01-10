#include "DataHelpers.hpp"

#include <stdexcept>

std::vector<std::string> DataHelpers::parseList(const std::string& line, const char sep, const bool ignoreEmpty)
{
	std::vector<std::string> result;
	size_t lastComma = -1;

	for (size_t i = 0; i < line.size(); ++i)
		if (line[i] == sep)
		{
			if (ignoreEmpty == false || i - lastComma - 1 > 0)
				result.emplace_back(std::move(line.substr(lastComma + 1, i - lastComma - 1)));
			lastComma = i;
		}
	if (ignoreEmpty == false || lastComma + 1 < line.size())
		result.emplace_back(std::move(line.substr(lastComma + 1)));
	return result;
}

std::vector<std::vector<std::string>> DataHelpers::parseLists(const std::string& line, const char outSep, const char inSep, const bool ignoreEmpty)
{
	const auto outer = parseList(line, outSep, ignoreEmpty);
	std::vector<std::vector<std::string>> result;
	result.reserve(outer.size());

	for (size_t i = 0; i < outer.size(); ++i)
	{
		const auto t = parseList(outer[i], inSep, ignoreEmpty);

		if(ignoreEmpty == false || t.size())
			result.emplace_back(std::move(t));
	}

	return result;
}

std::optional<int> DataHelpers::toInt(const std::string& str)
{
	if (str.empty())
		return std::nullopt;

	try
	{
		return std::optional<int>(std::stoi(str));
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

std::optional<unsigned int> DataHelpers::toUInt(const std::string& str)
{
	const auto r = toInt(str);

	if (r.has_value() == false || r.value() < 0)
		return std::nullopt;

	return std::optional<unsigned int>(r.value());
}

std::optional<double> DataHelpers::toDouble(const std::string& str)
{
	if(str.empty())
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

std::optional<double> DataHelpers::toUDouble(const std::string& str)
{
	auto r = toDouble(str);

	if (r.has_value() == false || r.value() < 0)
		return std::nullopt;

	return r;
}

std::optional<std::pair<double, double>> DataHelpers::toPair(const std::string& str)
{
	const auto& pairStr = parseList(str, '@', true);
	if (pairStr.size() != 2)
		return std::nullopt;

	const auto val1 = toDouble(pairStr.front());
	if (val1.has_value() == false)
		return std::nullopt;

	const auto val2 = toDouble(pairStr.back());
	if (val2.has_value() == false)
		return std::nullopt;

	return std::optional<std::pair<double, double>>(std::make_pair(val1.value(), val2.value()));
}

std::optional<Amount<Unit::CELSIUS>> DataHelpers::toCelsius(const std::string& str)
{
	const bool missingUnit = isdigit(str.back());

	const auto temp = missingUnit ?
		DataHelpers::toDouble(str) :
		DataHelpers::toDouble(str.substr(0, str.size() - 1));
	if (temp.has_value() == false)
		return std::nullopt;

	Amount<Unit::CELSIUS> cTemp = 0;
	if (str.ends_with('c') || str.ends_with('C') || missingUnit)
		cTemp = temp.value();
	else if (str.ends_with('k') || str.ends_with('K'))
		cTemp = Amount<Unit::KELVIN>(temp.value());
	else if (str.ends_with('f') || str.ends_with('F'))
		cTemp = Amount<Unit::FAHRENHEIT>(temp.value());

	return std::optional<Amount<Unit::CELSIUS>>(cTemp);
}

std::optional<Spline<float>> DataHelpers::toSpline(const std::string& str)
{
	const auto& pointsStr = parseList(str, ';', true);

	std::vector<std::pair<float, float>> points;
	points.reserve(pointsStr.size());

	for (size_t i = 0; i < pointsStr.size(); ++i)
	{
		const auto p = toPair(pointsStr[i]);
		if (p.has_value() == false)
			return std::nullopt;
		points.emplace_back(p.value());
	}

	return points.empty() ? std::nullopt : std::optional<Spline<float>>(Spline(std::move(points)));
}