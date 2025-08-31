#include "io/StringTable.hpp"

#include "global/Charset.hpp"
#include "io/Log.hpp"

#include <numeric>
#include <sstream>

StringTable::StringTable(std::vector<std::string>&& header, const bool alignEntriesLeft) noexcept :
    alignEntriesLeft(alignEntriesLeft),
    header(std::move(header))
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

    for (size_t i = 0; i < entries.size(); ++i)
        getMaxWidths(entries[i], widths);

    // make sure header can be cleanly centered
    for (size_t i = 0; i < header.size(); ++i)
        if ((widths[i] - header[i].size()) & 1)
            ++widths[i];

    return widths;
}

void StringTable::addEntry(std::vector<std::string>&& entry)
{
    if (entry.size() != header.size()) {
        Log(this).fatal("New entry size: {0} does not match the table header size: {1}.", entry.size(), header.size());
        return;
    }
    entries.emplace_back(std::move(entry));
}

void StringTable::addBreakline() { entries.emplace_back(std::vector<std::string>()); }

void StringTable::clear()
{
    header.clear();
    entries.clear();
}

void StringTable::dump(std::ostream& out) const
{
    const auto widths = getMaxWidths();

    // top line
    out << ASCII::CornerTopLeft;
    for (size_t i = 0; i < widths.size() - 1; ++i)
        out << std::string(widths[i], ASCII::LineH) << ASCII::JunctionDown;
    out << std::string(widths.back(), ASCII::LineH) << ASCII::CornerTopRight;
    out << '\n';

    // header
    out << ASCII::LineV;
    for (size_t i = 0; i < header.size(); ++i) {
        const auto pad = (widths[i] - header[i].size()) / 2.0f;
        out << std::string(static_cast<size_t>(std::ceil(pad)), ' ') << header[i]
            << std::string(static_cast<size_t>(pad), ' ') << ASCII::LineV;
    }
    out << '\n';

    // breakline
    out << ASCII::JunctionRight;
    for (size_t i = 0; i < widths.size() - 1; ++i)
        out << std::string(widths[i], ASCII::LineH) << ASCII::Cross;
    out << std::string(widths.back(), ASCII::LineH) << ASCII::JunctionLeft;
    out << '\n';

    for (size_t i = 0; i < entries.size(); ++i) {
        if (entries[i].empty()) {
            // breakline
            out << ASCII::JunctionRight;
            for (size_t j = 0; j < widths.size() - 1; ++j)
                out << std::string(widths[j], ASCII::LineH) << ASCII::Cross;
            out << std::string(widths.back(), ASCII::LineH) << ASCII::JunctionLeft;
            out << '\n';

            continue;
        }

        // entry
        out << ASCII::LineV;
        if (alignEntriesLeft) {
            for (size_t j = 0; j < entries[i].size(); ++j)
                out << ' ' << entries[i][j] << std::string(widths[j] - entries[i][j].size() - 1, ' ') << ASCII::LineV;
        }
        else {
            for (size_t j = 0; j < entries[i].size(); ++j)
                out << std::string(widths[j] - entries[i][j].size() - 1, ' ') << entries[i][j] << ' ' << ASCII::LineV;
        }

        out << '\n';
    }

    // bottom line
    out << ASCII::CornerBottomLeft;
    for (size_t i = 0; i < widths.size() - 1; ++i)
        out << std::string(widths[i], ASCII::LineH) << ASCII::JunctionUp;
    out << std::string(widths.back(), ASCII::LineH) << ASCII::CornerBottomRight;
}

std::string StringTable::toString() const
{
    std::stringstream out;
    dump(out);
    return out.str();
}

void StringTable::print() const { dump(std::cout); }
