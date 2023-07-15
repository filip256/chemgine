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