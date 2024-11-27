#include "StringTable.hpp"
#include "Log.hpp"

#include <numeric>
#include <sstream>

StringTable::StringTable(
	std::vector<std::string>&& header,
	const bool alignEntriesLeft
) noexcept :
	header(std::move(header)),
	alignEntriesLeft(alignEntriesLeft)
{}

void StringTable::getMaxWidths(const std::vector<std::string>& strings, std::vector<size_t>& widths)
{
	for (size_t i = 0; i < strings.size(); ++i)
		widths[i] = std::max(widths[i], strings[i].size() + 2);
}

std::vector<size_t> StringTable::getMaxWidths() const
{
	std::vector<size_t> widths(header.size(), 0);
	getMaxWidths(header, widths);

	for(size_t i = 0; i < entries.size(); ++i)
		getMaxWidths(entries[i], widths);

	// make sure header can be cleanly centered
	for (size_t i = 0; i < header.size(); ++i)
		if ((widths[i] - header[i].size()) & 1)
			++widths[i];

	return widths;
}

void StringTable::addEntry(std::vector<std::string>&& entry)
{
	if (entry.size() != header.size())
	{
		Log(this).fatal("New entry size: {0} does not match the table header size: {1}.", entry.size(), header.size());
		return;
	}
	entries.emplace_back(std::move(entry));
}

void StringTable::addBreakline()
{
	entries.emplace_back(std::vector<std::string>());
}

void StringTable::clear()
{
	header.clear();
	entries.clear();
}

void StringTable::dump(std::ostream& out) const
{
	const auto widths = getMaxWidths();
	const auto totalWidth = std::accumulate(widths.begin(), widths.end(), 0);

	// top line
	out << 'Ú';
	for (size_t i = 0; i < widths.size() - 1; ++i)
		out << std::string(widths[i], 'Ä') << 'Â';
	out << std::string(widths.back(), 'Ä') << '¿';
	out << '\n';

	// header
	out << '³';
	for (size_t i = 0; i < header.size(); ++i)
	{
		const auto pad = (widths[i] - header[i].size()) / 2.0f;
		out << std::string(std::ceil(pad), ' ') << header[i] << std::string(std::floor(pad), ' ') << '³';
	}
	out << '\n';

	// breakline
	out << 'Ã';
	for (size_t i = 0; i < widths.size() - 1; ++i)
		out << std::string(widths[i], 'Ä') << 'Å';
	out << std::string(widths.back(), 'Ä') << '´';
	out << '\n';

	for (size_t i = 0; i < entries.size(); ++i)
	{
		if (entries[i].empty())
		{
			// breakline
			out << 'Ã';
			for (size_t i = 0; i < widths.size() - 1; ++i)
				out << std::string(widths[i], 'Ä') << 'Å';
			out << std::string(widths.back(), 'Ä') << '´';
			out << '\n';

			continue;
		}

		// entry
		out << '³';
		if (alignEntriesLeft)
		{
			for (size_t j = 0; j < entries[i].size(); ++j)
				out << ' ' << entries[i][j] << std::string(widths[j] - entries[i][j].size() - 1, ' ') << '³';
		}
		else
		{
			for (size_t j = 0; j < entries[i].size(); ++j)
				out << std::string(widths[j] - entries[i][j].size() - 1, ' ') << entries[i][j] << " ³";
		}

		out << '\n';
	}

	// bottom line
	out << 'À';
	for (size_t i = 0; i < widths.size() - 1; ++i)
		out << std::string(widths[i], 'Ä') << 'Á';
	out << std::string(widths.back(), 'Ä') << 'Ù';
}

std::string StringTable::toString() const
{
	std::stringstream out;
	dump(out);
	return out.str();
}

void StringTable::print() const
{
	dump(std::cout);
}
