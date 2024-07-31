#include "StringUtils.hpp"

static std::pair<size_t, size_t> getStripInterval(const std::string& str, bool (*pred)(char))
{
	size_t start = 0, end = str.size();
	while (start < end && pred(str[start]))
		++start;
	while (end > start && pred(str[end - 1]))
		--end;

	return std::make_pair(start, end);
}

void Utils::strip(std::string& str, bool (*pred)(char))
{
	const auto interval = getStripInterval(str, pred);
    str = str.substr(interval.first, interval.second - interval.first);
}

std::string Utils::strip(const std::string& str, bool (*pred)(char))
{
	const auto interval = getStripInterval(str, pred);
	return str.substr(interval.first, interval.second - interval.first);
}

std::vector<std::string> Utils::split(const std::string& line, const char separator, const bool ignoreEmpty)
{
	std::vector<std::string> result;
	size_t lastSep = static_cast<size_t>(-1);

	for (size_t i = 0; i < line.size(); ++i)
	{
		if (line[i] != separator)
			continue;

		if (i - lastSep - 1 > 0)
		{
			auto item = Utils::strip(line.substr(lastSep + 1, i - lastSep - 1));
			if (ignoreEmpty == false || item.size())
				result.emplace_back(std::move(item));
		}
		else if (ignoreEmpty == false)
			result.emplace_back();

		lastSep = i;
	}

	if (lastSep + 1 < line.size())
	{
		auto item = Utils::strip(line.substr(lastSep + 1));
		if (ignoreEmpty == false || item.size())
			result.emplace_back(std::move(item));
	}
	else if (ignoreEmpty == false)
		result.emplace_back();

	return result;
}

std::vector<std::string> Utils::split(
	const std::string& line,
	const char separator,
	const char ignoreSectionBegin,
	const char ignoreSectionEnd,
	const bool ignoreEmpty)
{
	std::vector<std::string> result;
	size_t lastSep = static_cast<size_t>(-1);
	bool ignoreSection = false;

	for (size_t i = 0; i < line.size(); ++i)
	{
		if (line[i] == ignoreSectionBegin)
			ignoreSection = true;
		else if(line[i] == ignoreSectionEnd)
			ignoreSection = false;

		if (ignoreSection)
			continue;

		if (line[i] != separator)
			continue;

		if (i - lastSep - 1 > 0)
		{
			auto item = Utils::strip(line.substr(lastSep + 1, i - lastSep - 1));
			if (ignoreEmpty == false || item.size())
				result.emplace_back(std::move(item));
		}
		else if (ignoreEmpty == false)
			result.emplace_back();

		lastSep = i;
	}

	if (lastSep + 1 < line.size())
	{
		auto item = Utils::strip(line.substr(lastSep + 1));
		if (ignoreEmpty == false || item.size())
			result.emplace_back(std::move(item));
	}
	else if (ignoreEmpty == false)
		result.emplace_back();

	return result;
}

std::vector<std::vector<std::string>> Utils::splitLists(const std::string& line, const char outerSeparator, const char innerSeparator, const bool ignoreEmpty)
{
	const auto outer = Utils::split(line, outerSeparator, ignoreEmpty);
	std::vector<std::vector<std::string>> result;
	result.reserve(outer.size());

	for (size_t i = 0; i < outer.size(); ++i)
	{
		auto t = split(outer[i], innerSeparator, ignoreEmpty);
		if (ignoreEmpty == false || t.size())
			result.emplace_back(std::move(t));
	}

	return result;
}
