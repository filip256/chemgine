#pragma once

#include "data/def/PrintSettings.hpp"
#include "data/def/Printers.hpp"
#include "estimators/kinds/UnitizedEstimator.hpp"
#include "utils/Meta.hpp"

#include <ostream>

namespace def
{

class DataDumper
{
private:
    std::string               baseIndent;
    std::ostream&             out;
    const def::PrintSettings& settings;
    const uint8_t             valueOffset;
    bool                      missingSep = false;

public:
    DataDumper(std::ostream& out, const uint8_t valueOffset, const uint16_t baseIndent, const bool prettify) noexcept;
    DataDumper(const DataDumper&) = default;
    DataDumper(DataDumper&&)      = default;

    template <typename T>
    DataDumper& header(const std::string_view type, const T& specifier, const std::string& idendtifier);

    template <typename T>
    DataDumper& property(const std::string_view name, const T& value);
    template <typename T>
    DataDumper& defaultProperty(const std::string_view name, const T& value, const T& defaultValue);

    template <typename T, typename HistoryT>
    DataDumper& tryOutlineSubDefinition(const T& value, HistoryT& alreadyPrinted);
    template <typename T, typename HistoryT>
    DataDumper& subDefinition(const std::string_view name, const T& value, HistoryT& alreadyPrinted);

    DataDumper& beginProperties();
    DataDumper& endProperties();
    DataDumper& endDefinition();
};

template <typename T>
DataDumper& DataDumper::header(const std::string_view type, const T& specifier, const std::string& identifier)
{
    out << '_' << type;
    if (identifier.size())
        out << '<' << identifier << '>';

    out << settings.specifierSep << (settings.prettify ? def::prettyPrint(specifier) : def::print(specifier));
    return *this;
}

template <typename T>
DataDumper& DataDumper::property(const std::string_view name, const T& value)
{
    if (missingSep)
        out << settings.propertySep << settings.newLine;
    missingSep = true;

    if (settings.prettify)
        out << baseIndent;
    out << settings.propertyIndent << name << ':';

    if (settings.prettify)
        out << std::string(valueOffset - name.size(), ' ') << def::prettyPrint(value);
    else
        out << def::print(value);

    return *this;
}

template <typename T>
DataDumper& DataDumper::defaultProperty(const std::string_view name, const T& value, const T& defaultValue)
{
    if (not utils::equal(value, defaultValue))
        property(name, value);
    return *this;
}

template <typename T, typename HistoryT>
DataDumper& DataDumper::tryOutlineSubDefinition(const T& value, HistoryT& alreadyPrinted)
{
    value->dumpDefinition(out, settings.prettify, alreadyPrinted, false, 0);
    return *this;
}

template <typename T, typename HistoryT>
DataDumper& DataDumper::subDefinition(const std::string_view name, const T& value, HistoryT& alreadyPrinted)
{
    if (missingSep)
        out << settings.propertySep << settings.newLine;
    missingSep = true;

    if (settings.prettify)
        out << baseIndent;
    out << settings.propertyIndent << name << ':';

    if (settings.prettify)
        out << std::string(valueOffset - name.size(), ' ');

    value->dumpDefinition(
        out,
        settings.prettify,
        alreadyPrinted,
        true,
        checked_cast<uint8_t>(valueOffset + settings.propertyIndent.size() + 1));

    return *this;
}

}  // namespace def
