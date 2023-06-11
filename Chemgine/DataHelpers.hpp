#pragma once

#include "Result.hpp"

#include <vector>
#include <string>

class DataHelpers
{
public:
	/// <summary>
	/// Parses a string consisting of words delimited by a character  and returns a vector of these words
	/// </summary>
	static std::vector<std::string> parseList(const std::string& csvLine, const char sep);


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
};