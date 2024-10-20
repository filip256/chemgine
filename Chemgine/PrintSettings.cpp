#include "PrintSettings.hpp"

using namespace Def;

const PrintSettings PrintSettings::Minimal = PrintSettings(false, "", ":", ",", "{", "}", ";\n");
const PrintSettings PrintSettings::Pretty = PrintSettings(true, "    ", ": ", ",\n", " {\n", "\n}", ";\n\n");

PrintSettings::PrintSettings(
	const bool prettify,
	std::string&& indent,
	std::string&& specifierSep,
	std::string&& propertySep,
	std::string&& propertyBlockBegin,
	std::string&& propertyBlockEnd,
	std::string&& definitionEnd
) noexcept :
	prettify(prettify),
	indent(indent),
	specifierSep(std::move(specifierSep)),
	propertySep(std::move(propertySep)),
	propertyBlockBegin(std::move(propertyBlockBegin)),
	propertyBlockEnd(std::move(propertyBlockEnd)),
	definitionEnd(std::move(definitionEnd))
{}
