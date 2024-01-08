#pragma once

#include "Amount.hpp"
#include "Spline.hpp"

#include <vector>
#include <string>
#include <optional>

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
	static std::optional<int> toInt(const std::string& str);

	/// <summary>
	/// Converts a string to an unsigned integer
	/// </summary>
	static std::optional<unsigned int> toUInt(const std::string& str);

	/// <summary>
	/// Converts a string to a double
	/// </summary>
	static std::optional<double> toDouble(const std::string& str);

	/// <summary>
	/// Converts a string to a double, checking that it's non-negative
	/// </summary>
	static std::optional<double> toUDouble(const std::string& str);

	/// <summary>
	/// Converts a string to a double, checking that it's non-negative
	/// </summary>
	static std::optional<Amount<Unit::CELSIUS>> toCelsius(const std::string& str);

	/// <summary>
	/// Converts a string to a pair of values
	/// Format: 1.1@24.5
	/// </summary>
	static std::optional<std::pair<double, double>> toPair(const std::string& str);

	/// <summary>
	/// Converts a string to a pair of amounts representing a property at a certain temperature
	/// Format: 1.1@24.5C
	/// </summary>
	template <Unit T>
	static std::optional<std::pair<Amount<T>, Amount<Unit::CELSIUS>>> toValueAtTemperature(const std::string& str);

	/// <summary>
	/// Converts a string to a spline
	/// Format: 20.5@100;6.4@30.3;...
	/// </summary>
	static std::optional<Spline<float>> toSpline(const std::string& str);
};


template <Unit T>
static std::optional<std::pair<Amount<T>, Amount<Unit::CELSIUS>>> DataHelpers::toValueAtTemperature(const std::string& str)
{
	const auto& pairStr = parseList(str, '@', true);
	if (pairStr.size() != 2)
		return std::nullopt;

	const auto val = toDouble(pairStr.front());
	if (val.has_value() == false)
		return std::nullopt;

	const auto temp = toCelsius(pairStr.back());
	if (val.has_value() == false)
		return std::nullopt;

	return std::optional<std::pair<Amount<T>, Amount<Unit::CELSIUS>>>(std::make_pair(Amount<T>(val.value()), temp.value()));
}