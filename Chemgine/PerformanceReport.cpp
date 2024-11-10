#include "PerformanceReport.hpp"
#include "PathUtils.hpp"
#include "Precision.hpp"
#include "Printers.hpp"
#include "Parsers.hpp"
#include "Log.hpp"

#include <fstream>

void PerformanceReport::add(const std::string& key, const std::chrono::nanoseconds& time)
{
	auto it = timeTable.find(key);
	if (it != timeTable.end())
	{
		it->second += time;
		return;
	}

	timeTable.emplace(key, time);
}

void PerformanceReport::merge(PerformanceReport&& other)
{
	this->timeTable.merge(std::move(other.timeTable));
	for (const auto& [k, t] : other.timeTable)
		this->timeTable.at(k) += t;
}

void PerformanceReport::setTimestamp()
{
	const auto now = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
	timestamp = std::format("{:%Y-%m-%d-%H-%M-%S}", now);
}

void PerformanceReport::clear()
{
	timestamp = "";
	timeTable.clear();
}

bool PerformanceReport::load(const std::string& path)
{
	std::ifstream file(path);
	if (not file.is_open())
	{
		Log(this).error("Failed to open file: '{0}' for reading.", path);
		return false;
	}

	if (not std::getline(file, timestamp))
	{
		Log(this).error("Report file: '{0}' is empty.", path);
		return false;
	}

	std::string line;
	while (std::getline(file, line))
	{
		const auto pair = Def::parse<std::pair<std::string, uint64_t>>(line);
		if (not pair)
		{
			Log(this).error("Invalid report line: '{0}' in file: '{1}'.", line, path);
			continue;
		}

		add(pair->first, std::chrono::nanoseconds(pair->second));
	}

	file.close();
	return true;
}

void PerformanceReport::dump(std::ostream& out) const
{
	out << timestamp << '\n';
	for (const auto& [k, t] : timeTable)
		out << Def::print(std::pair(k, t.count())) << '\n';
}

void PerformanceReport::dump(const std::string& path) const
{
	std::ofstream out(path);
	if (not out.is_open())
	{
		Log(this).fatal("Failed to open file: '{0}' for writing.", path);
		return;
	}

	dump(out);
	out.close();
}

StringTable PerformanceReport::compare(const PerformanceReport& other) const
{
	StringTable table({ "Test Name", this->timestamp, other.timestamp, "Difference", "Change" }, true);

	for (const auto& [k, t] : this->timeTable)
	{
		const auto thisTimeStr = Linguistics::formatFloatingPoint(t.count() / 1000.0) + "us";

		const auto oth = other.timeTable.find(k);
		if (oth == other.timeTable.end())
		{
			table.addEntry({ k, thisTimeStr, "?", "?", "?"});
			continue;
		}

		const auto difference = oth->second.count() - t.count();
		const auto change = (static_cast<float_h>(difference) / std::max(t.count(), 1LL)) * 100.0;

		const auto otherTimeStr = Linguistics::formatFloatingPoint(oth->second.count() / 1000.0) + "us";
		const auto differenceStr = Linguistics::formatFloatingPoint(difference / 1000.0) + "us";
		const auto changeStr = Linguistics::formatFloatingPoint(change) + '%';

		table.addEntry({ k, thisTimeStr, otherTimeStr, differenceStr, changeStr });
	}

	for (const auto& [k, t] : other.timeTable)
	{
		if (not this->timeTable.contains(k))
		{
			const auto otherTimeStr = Linguistics::formatFloatingPoint(t.count() / 1000.0) + "us";
			table.addEntry({ k, "?", otherTimeStr, "?", "?" });
		}
	}

	return table;
}
