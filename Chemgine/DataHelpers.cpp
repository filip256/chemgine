#include "DataHelpers.hpp"

#include <stdexcept>

std::vector<std::string> DataHelpers::parseList(const std::string& csvLine, const char sep)
{
	std::vector<std::string> outVector;

	const std::string::size_type size = csvLine.size();
	std::string::size_type lastComma = -1;

	for (std::string::size_type i = 0; i < size; ++i)
		if (csvLine[i] == sep)
		{
			outVector.emplace_back(csvLine.substr(lastComma + 1, i - lastComma - 1));
			lastComma = i;
		}
	outVector.emplace_back(csvLine.substr(lastComma + 1));
	return outVector;
}

Result<int> DataHelpers::toInt(const std::string& str)
{
	try
	{
		return Result<int>(std::stoi(str));
	}
	catch (std::invalid_argument const& ex)
	{
		return Result<int>();
	}
	catch (std::out_of_range const& ex)
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
	catch (std::invalid_argument const& ex)
	{
		return Result<double>();
	}
	catch (std::out_of_range const& ex)
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