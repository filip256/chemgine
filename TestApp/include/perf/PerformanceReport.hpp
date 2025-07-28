#pragma once

#include "StringTable.hpp"
#include "perf/TimingResult.hpp"

#include <string>
#include <chrono>
#include <map>

class PerformanceReport
{
private:
	std::string timestamp = "";
	std::map<std::string, TimingResult> timeTable;

public:
	PerformanceReport() = default;
	PerformanceReport(const PerformanceReport&) = default;
	PerformanceReport(PerformanceReport&&) = default;

	void add(const std::string& key, const TimingResult& time);
	void merge(PerformanceReport&& other);
	void setTimestamp();

	void clear();

	bool load(const std::string& path);
	void dump(std::ostream& out) const;
	void dump(const std::string& path) const;

	StringTable compare(const PerformanceReport& other) const;
};
