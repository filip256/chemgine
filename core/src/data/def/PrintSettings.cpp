#include "data/def/PrintSettings.hpp"

using namespace def;

const PrintSettings PrintSettings::Minimal = PrintSettings(false, "", ":", ",", "", "{", "}", ";\n");
const PrintSettings PrintSettings::Pretty = PrintSettings(true, "\n", ": ", ",", "    ", " {", "}", ";\n");

PrintSettings::PrintSettings(
	const bool prettify,
	std::string&& newLine,
	std::string&& specifierSep,
	std::string&& propertySep,
	std::string&& propertyIndent,
	std::string&& propertyBlockBegin,
	std::string&& propertyBlockEnd,
	std::string&& definitionEnd
) noexcept :
	prettify(std::move(prettify)),
	newLine(std::move(newLine)),
	specifierSep(std::move(specifierSep)),
	propertySep(std::move(propertySep)),
	propertyIndent(std::move(propertyIndent)),
	propertyBlockBegin(std::move(propertyBlockBegin)),
	propertyBlockEnd(std::move(propertyBlockEnd)),
	definitionEnd(std::move(definitionEnd))
{}
