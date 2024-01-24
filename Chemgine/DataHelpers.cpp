#include "DataHelpers.hpp"

std::vector<std::string> DataHelpers::parseList(const std::string& line, const char sep, const bool ignoreEmpty)
{
	std::vector<std::string> result;
	size_t lastSep = -1;

	for (size_t i = 0; i < line.size(); ++i)
		if (line[i] == sep)
		{
			if (ignoreEmpty == false || i - lastSep - 1 > 0)
				result.emplace_back(std::move(line.substr(lastSep + 1, i - lastSep - 1)));
			lastSep = i;
		}
	if (ignoreEmpty == false || lastSep + 1 < line.size())
		result.emplace_back(std::move(line.substr(lastSep + 1)));
	return result;
}

std::vector<std::vector<std::string>> DataHelpers::parseLists(const std::string& line, const char outSep, const char inSep, const bool ignoreEmpty)
{
	const auto outer = parseList(line, outSep, ignoreEmpty);
	std::vector<std::vector<std::string>> result;
	result.reserve(outer.size());

	for (size_t i = 0; i < outer.size(); ++i)
	{
		auto t = parseList(outer[i], inSep, ignoreEmpty);
		if(ignoreEmpty == false || t.size())
			result.emplace_back(std::move(t));
	}

	return result;
}