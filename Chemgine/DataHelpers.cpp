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
	result.emplace_back(std::move(line.substr(lastComma + 1)));
	return result;
}

Result<int> DataHelpers::toInt(const std::string& str)
{
	try
	{
		return Result<int>(std::stoi(str));
	}
	catch (const std::invalid_argument&)
	{
		return Result<int>();
	}
	catch (const std::out_of_range&)
	{
		return Result<int>();
	}
}

Result<unsigned int> DataHelpers::toUInt(const std::string& str)
{
	const auto r = toInt(str);

	if (r.status == 0 || r.result < 0)
		return Result<unsigned int>();

	return Result<unsigned int>(r.result);
}

Result<double> DataHelpers::toDouble(const std::string& str)
{
	try
	{
		return Result<double>(std::stod(str));
	}
	catch (const std::invalid_argument&)
	{
		return Result<double>();
	}
	catch (const std::out_of_range&)
	{
		return Result<double>();
	}
}

Result<double> DataHelpers::toUDouble(const std::string& str)
{
	auto r = toDouble(str);

	if (r.status == 0 || r.result < 0)
		return Result<double>();

	return r;
}

Result<std::pair<double, double>> DataHelpers::toPair(const std::string& str)
{
	const auto& pairStr = parseList(str, '@', true);
	if (pairStr.size() != 2)
		return Result<std::pair<double, double>>();

	const auto val1 = toDouble(pairStr.front());
	if (val1.status == 0)
		return Result<std::pair<double, double>>();

	const auto val2 = toDouble(pairStr.back());
	if (val2.status == 0)
		return Result<std::pair<double, double>>();

	return Result<std::pair<double, double>>(std::make_pair(val1.result, val2.result));
}

Result<Amount<Unit::CELSIUS>> DataHelpers::toCelsius(const std::string& str)
{
	const bool missingUnit = isdigit(str.back());

	const auto temp = missingUnit ?
		DataHelpers::toDouble(str) :
		DataHelpers::toDouble(str.substr(0, str.size() - 1));
	if (temp.status == 0)
		return Result<Amount<Unit::CELSIUS>>();

	Amount<Unit::CELSIUS> cTemp = 0;
	if (str.ends_with('c') || str.ends_with('C') || missingUnit)
		cTemp = temp.result;
	else if (str.ends_with('k') || str.ends_with('K'))
		cTemp = Amount<Unit::KELVIN>(temp.result);
	else if (str.ends_with('f') || str.ends_with('F'))
		cTemp = Amount<Unit::FAHRENHEIT>(temp.result);

	return Result<Amount<Unit::CELSIUS>>(cTemp);
}

Result<Spline<float>> DataHelpers::toSpline(const std::string& str)
{
	const auto& pointsStr = parseList(str, ';', true);

	std::vector<std::pair<float, float>> points;
	points.reserve(pointsStr.size());

	for (size_t i = 0; i < pointsStr.size(); ++i)
	{
		const auto p = toPair(pointsStr[i]);
		if (p.status == 0)
			return Result<Spline<float>>();
		points.emplace_back(p.result);
	}

	return Result<Spline<float>>(Spline(std::move(points)));
}