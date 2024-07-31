#pragma once

#include <string>
#include <vector>

namespace Utils
{
	void strip(
		std::string& str,
		bool (*pred)(const char) = [](const auto c) -> bool { return std::isspace(c); });

	std::string strip(
		const std::string& str,
		bool (*pred)(const char) = [](const auto c) -> bool { return std::isspace(c); });

	std::vector<std::string> split(
		const std::string& line,
		const char separator,
		const bool ignoreEmpty = false);

	std::vector<std::string> split(
		const std::string& line,
		const char separator,
		const char ignoreSectionBegin,
		const char ignoreSectionEnd,
		const bool ignoreEmpty = false);

	std::vector<std::vector<std::string>> splitLists(
		const std::string& line,
		const char outerSeparator,
		const char innerSeparator,
		const bool ignoreEmpty = false);
};
