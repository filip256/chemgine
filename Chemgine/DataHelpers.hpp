#pragma once

#include "Result.hpp"
#include "Amount.hpp"
#include "Spline.hpp"

#include <vector>
#include <string>

class DataHelpers
{
public:
	/// <summary>
	/// Parses a string consisting of words delimited by a character  and returns a vector of these words
	/// </summary>
	static std::vector<std::string> parseList(
		const std::string& csvLine,
		const char sep,
		const bool ignoreEmpty = false);


	/// <summary>
	/// Converts a string to an integer
	/// </summary>
	static Result<int> toInt(const std::string& str);

	/// <summary>
	/// Converts a string to an unsigned integer
	/// </summary>
	static Result<unsigned int> toUInt(const std::string& str);

	/// <summary>
	/// Converts a string to a double
	/// </summary>
	static Result<double> toDouble(const std::string& str);

	/// <summary>
	/// Converts a string to a double, checking that it's non-negative
	/// </summary>
	static Result<double> toUDouble(const std::string& str);

	/// <summary>
	/// Converts a string to a double, checking that it's non-negative
	/// </summary>
	static Result<Amount<Unit::CELSIUS>> toCelsius(const std::string& str);

	/// <summary>
	/// Converts a string to a pair of values
	/// Format: 1.1@24.5
	/// </summary>
	static Result<std::pair<double, double>> toPair(const std::string& str);

	/// <summary>
	/// Converts a string to a pair of amounts representing a property at a certain temperature
	/// Format: 1.1@24.5C
	/// </summary>
	template <Unit T>
	static Result<std::pair<Amount<T>, Amount<Unit::CELSIUS>>> toValueAtTemperature(const std::string& str);

	/// <summary>
	/// Converts a string to a spline
	/// Format: 20.5@100;6.4@30.3;...
	/// </summary>
	static Result<Spline<float>> toSpline(const std::string& str);
};


template <Unit T>
static Result<std::pair<Amount<T>, Amount<Unit::CELSIUS>>> DataHelpers::toValueAtTemperature(const std::string& str)
{
	const auto& pairStr = parseList(str, '@', true);
	if (pairStr.size() != 2)
		return Result<std::pair<Amount<T>, Amount<Unit::CELSIUS>>>();

	const auto val = toDouble(pairStr.front());
	if (val.status == 0)
		return Result<std::pair<Amount<T>, Amount<Unit::CELSIUS>>>();

	const auto temp = toCelsius(pairStr.back());
	if (temp.status == 0)
		return Result<std::pair<Amount<T>, Amount<Unit::CELSIUS>>>();

	return Result<std::pair<Amount<T>, Amount<Unit::CELSIUS>>>(std::make_pair(Amount<T>(val.result), temp.result));
}