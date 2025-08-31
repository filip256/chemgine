#include "io/StringTable.hpp"

#include "global/Charset.hpp"
#include "io/Log.hpp"

#include <numeric>
#include <sstream>

using namespace details;

template <typename StringT>
Table<StringT>::Table(std::vector<StringT>&& header, const bool alignEntriesLeft) noexcept :
    header(std::move(header)),
    alignEntriesLeft(alignEntriesLeft)
{}

template <typename StringT>
void Table<StringT>::getMaxWidths(const std::vector<StringT>& strings, std::vector<size_t>& widths)
{
    for (size_t i = 0; i < strings.size(); ++i)
        widths[i] = std::max(widths[i], strings[i].size() + 2);
}

template <typename StringT>
std::vector<size_t> Table<StringT>::getMaxWidths() const
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

template <typename StringT>
void Table<StringT>::addEntry(std::vector<StringT>&& entry)
{
    if (entry.size() != header.size()) {
        Log(this).fatal("New entry size: {0} does not match the table header size: {1}.", entry.size(), header.size());
        return;
    }
    entries.emplace_back(std::move(entry));
}

template <typename StringT>
void Table<StringT>::addBreakline()
{
    entries.emplace_back(std::vector<StringT>());
}

template <typename StringT>
void Table<StringT>::clear()
{
    header.clear();
    entries.clear();
}

template <typename StringT>
template <typename StreamT>
void Table<StringT>::dump(StreamT& out) const
{
    const auto widths = getMaxWidths();

    // top line
    out << ASCII::CornerTopLeft;
    for (size_t i = 0; i < widths.size() - 1; ++i)
        out << StringT(widths[i], ASCII::LineH) << ASCII::JunctionDown;
    out << StringT(widths.back(), ASCII::LineH) << ASCII::CornerTopRight;
    out << '\n';

    // header
    out << ASCII::LineV;
    for (size_t i = 0; i < header.size(); ++i) {
        const auto pad = (widths[i] - header[i].size()) / 2.0f;
        out << StringT(static_cast<size_t>(std::ceil(pad)), ' ') << header[i] << StringT(static_cast<size_t>(pad), ' ')
            << ASCII::LineV;
    }
    out << '\n';

    // breakline
    out << ASCII::JunctionRight;
    for (size_t i = 0; i < widths.size() - 1; ++i)
        out << StringT(widths[i], ASCII::LineH) << ASCII::Cross;
    out << StringT(widths.back(), ASCII::LineH) << ASCII::JunctionLeft;
    out << '\n';

    for (size_t i = 0; i < entries.size(); ++i) {
        if (entries[i].empty()) {
            // breakline
            out << ASCII::JunctionRight;
            for (size_t j = 0; j < widths.size() - 1; ++j)
                out << StringT(widths[j], ASCII::LineH) << ASCII::Cross;
            out << StringT(widths.back(), ASCII::LineH) << ASCII::JunctionLeft;
            out << '\n';

            continue;
        }

        // entry
        out << ASCII::LineV;
        if (alignEntriesLeft) {
            for (size_t j = 0; j < entries[i].size(); ++j)
                out << ' ' << entries[i][j] << StringT(widths[j] - entries[i][j].size() - 1, ' ') << ASCII::LineV;
        }
        else {
            for (size_t j = 0; j < entries[i].size(); ++j)
                out << StringT(widths[j] - entries[i][j].size() - 1, ' ') << entries[i][j] << ' ' << ASCII::LineV;
        }

        out << '\n';
    }

    // bottom line
    out << ASCII::CornerBottomLeft;
    for (size_t i = 0; i < widths.size() - 1; ++i)
        out << StringT(widths[i], ASCII::LineH) << ASCII::JunctionUp;
    out << StringT(widths.back(), ASCII::LineH) << ASCII::CornerBottomRight;
}

template <typename StringT>
StringT Table<StringT>::toString() const
{
    std::stringstream out;
    dump(out);
    return out.str();
}

template <typename StringT>
void Table<StringT>::print() const
{
    dump(std::cout);
}

//
// Definitions
//

template class details::Table<std::string>;
template class details::Table<ColoredString>;

template void details::Table<std::string>::dump<std::ostream>(std::ostream&) const;
template void details::Table<std::string>::dump<std::ostringstream>(std::ostringstream&) const;
template void details::Table<ColoredString>::dump<std::ostream>(std::ostream&) const;
template void details::Table<ColoredString>::dump<ColoredString>(ColoredString&) const;
