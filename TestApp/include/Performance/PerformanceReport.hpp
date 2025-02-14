#pragma once

#include "StringTable.hpp"

#include <string>
#include <chrono>
#include <map>

class PerformanceReport
{
private:
	std::string timestamp = "";
	std::map<std::string, std::chrono::nanoseconds> timeTable;

public:
	PerformanceReport() = default;
	PerformanceReport(const PerformanceReport&) = default;
	PerformanceReport(PerformanceReport&&) = default;

	void add(const std::string& key, const std::chrono::nanoseconds& time);
	void merge(PerformanceReport&& other);
	void setTimestamp();

	void clear();

	bool load(const std::string& path);
	void dump(std::ostream& out) const;
	void dump(const std::string& path) const;

	StringTable compare(const PerformanceReport& other) const;
};
