#include "DataDumper.hpp"

DataDumper::DataDumper(
	std::ostream& out,
	const uint8_t valueOffset,
	const uint16_t baseIndent,
	const bool prettify
) noexcept :
	out(out),
	settings(prettify ? Def::PrintSettings::Pretty : Def::PrintSettings::Minimal),
	valueOffset(valueOffset + 2),
	baseIndent(std::string(baseIndent, ' '))
{}

DataDumper& DataDumper::beginProperties()
{
	out << settings.propertyBlockBegin << settings.newLine;
	return *this;
}

DataDumper& DataDumper::endProperties()
{
	out << settings.newLine << baseIndent << settings.propertyBlockEnd;
	return *this;
}

DataDumper& DataDumper::endDefinition()
{
	out << settings.definitionEnd << settings.newLine;
	return *this;
}
