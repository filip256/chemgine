#include "perf/PerformanceReport.hpp"
#include "utils/Path.hpp"
#include "global/Precision.hpp"
#include "data/def/Printers.hpp"
#include "data/def/Parsers.hpp"
#include "io/Log.hpp"

#include <fstream>
#include <array>
#include <algorithm>

void PerformanceReport::add(const std::string& key, const TimingResult& time)
{
	auto it = timeTable.find(key);
	if (it != timeTable.end())
		Log(this).fatal("Report already contains a time entry for: '{0}'", key);

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
		const auto pair = def::parse<std::pair<std::string, std::pair<int64_t, int64_t>>>(line);
		if (not pair)
		{
			Log(this).error("Invalid report line: '{0}' in file: '{1}'.", line, path);
			continue;
		}

		add(pair->first, TimingResult(
			std::chrono::nanoseconds(pair->second.first),
			std::chrono::nanoseconds(pair->second.second)));
	}

	file.close();
	return true;
}

void PerformanceReport::dump(std::ostream& out) const
{
	out << timestamp << '\n';
	for (const auto& [k, t] : timeTable)
		out << def::print(std::pair(k, std::pair(t.averageTime.count(), t.medianTime.count()))) << '\n';
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
	StringTable table({ "Test Name", this->timestamp, other.timestamp, "Difference", "Change"}, true);

	for (const auto& [k, t] : this->timeTable)
	{
		const auto oth = other.timeTable.find(k);
		if (oth == other.timeTable.end())
		{
			const auto thisTimeStr = std::format("{:.2f}",
				std::min(t.averageTime, t.medianTime).count() / 1000.0) + "us";
			table.addEntry({ k, thisTimeStr, "?", "?", "?" });
			continue;
		}

		const std::array<std::pair<float_h, float_h>, 4> pairs
		{
			std::pair(t.averageTime.count() / 1000.0, oth->second.averageTime.count() / 1000.0),
			std::pair(t.medianTime.count() / 1000.0, oth->second.medianTime.count() / 1000.0),
			std::pair(t.averageTime.count() / 1000.0, oth->second.medianTime.count() / 1000.0),
			std::pair(t.medianTime.count() / 1000.0, oth->second.averageTime.count() / 1000.0),
		};

		// Compute the % changes
		std::array<float_h, 4> changes;
		std::transform(pairs.begin(), pairs.end(), changes.begin(),
			[](const auto& p) { return (p.first - p.second) / std::max(p.second, std::numeric_limits<float_h>::min()) * 100.0; });
		
		// Pick the smallest absolute change out of the 4 combinations
		const auto minChangeIt = std::min_element(changes.begin(), changes.end(),
			[](const auto lhs, const auto rhs) { return std::abs(lhs) < std::abs(rhs); });

		const auto minIdx = std::distance(changes.begin(), minChangeIt);
		const auto thisTimeStr = std::format("{:.2f}", pairs[minIdx].first) + "us";
		const auto otherTimeStr = std::format("{:.2f}", pairs[minIdx].second) + "us";
		const auto minDiffStr = std::format("{:.2f}", pairs[minIdx].first - pairs[minIdx].second) + "us";
		const auto minChangeStr = std::format("{:+.2f}", *minChangeIt) + '%';

		table.addEntry({ k, thisTimeStr, otherTimeStr, minDiffStr, minChangeStr });
	}

	for (const auto& [k, t] : other.timeTable)
	{
		if (not this->timeTable.contains(k))
		{
			const auto otherTimeStr = std::format("{:.2f}",
				std::min(t.averageTime, t.medianTime).count() / 1000.0) + "us";
			table.addEntry({ k, "?", otherTimeStr, "?", "?"});
		}
	}

	return table;
}
