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
	indent(std::string(baseIndent, ' ') + settings.indent)
{}

DataDumper& DataDumper::beginProperties()
{
	out << settings.propertyBlockBegin;
	return *this;
}

DataDumper& DataDumper::endProperties()
{
	out << indent << settings.propertyBlockEnd;
	return *this;
}

DataDumper& DataDumper::endDefinition()
{
	out << settings.definitionEnd;
	return *this;
}
