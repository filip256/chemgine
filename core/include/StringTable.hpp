#pragma once

#include <string>
#include <vector>

class StringTable
{
private:
	const bool alignEntriesLeft;
	std::vector<std::string> header;
	std::vector<std::vector<std::string>> entries;

	static void getMaxWidths(const std::vector<std::string>& strings, std::vector<size_t>& widths);
	std::vector<size_t> getMaxWidths() const;

public:
	StringTable(
		std::vector<std::string>&& header,
		const bool alignEntriesLeft
	) noexcept;
	StringTable(const StringTable&) = default;
	StringTable(StringTable&&) = default;

	void addEntry(std::vector<std::string>&& entry);
	void addBreakline();

	void clear();

	void dump(std::ostream& out) const;
	std::string toString() const;
	void print() const;
};
