#pragma once

#include "io/ColoredString.hpp"

#include <string>
#include <vector>

namespace details
{

template <typename StringT>
class Table
{
private:
    std::vector<StringT>              header;
    std::vector<std::vector<StringT>> entries;
    const bool                        alignEntriesLeft;

    static void         getMaxWidths(const std::vector<StringT>& strings, std::vector<size_t>& widths);
    std::vector<size_t> getMaxWidths() const;

public:
    Table(std::vector<StringT>&& header, const bool alignEntriesLeft) noexcept;
    Table(const Table&) = default;
    Table(Table&&)      = default;

    void addEntry(std::vector<StringT>&& entry);
    void addBreakline();

    void clear();

    template <typename StreamT>
    void    dump(StreamT& out) const;
    StringT toString() const;
    void    print() const;

    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, const Table<U>& table);
};

template <typename StringT>
std::ostream& operator<<(std::ostream& os, const Table<StringT>& table)
{
    table.dump(os);
    return os;
}

}  // namespace details

using StringTable        = details::Table<std::string>;
using ColoredStringTable = details::Table<ColoredString>;
