#include "data/def/DataDumper.hpp"

using namespace def;

DataDumper::DataDumper(
    std::ostream& out, const uint8_t valueOffset, const uint16_t baseIndent, const bool prettify) noexcept :
    baseIndent(std::string(baseIndent, ' ')),
    out(out),
    settings(prettify ? def::PrintSettings::Pretty : def::PrintSettings::Minimal),
    valueOffset(valueOffset + 2)
{}

DataDumper& DataDumper::beginProperties()
{
    out << settings.propertyBlockBegin << settings.newLine;
    return *this;
}

DataDumper& DataDumper::endProperties()
{
    missingSep = false;

    out << settings.newLine;
    if (settings.prettify)
        out << baseIndent;
    out << settings.propertyBlockEnd;
    return *this;
}

DataDumper& DataDumper::endDefinition()
{
    out << settings.definitionEnd << settings.newLine;
    return *this;
}
