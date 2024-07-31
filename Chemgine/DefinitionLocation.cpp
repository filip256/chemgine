#include "DefinitionLocation.hpp"

DefinitionLocation::DefinitionLocation(
	const std::string& file,
	const size_t line
) noexcept :
	file(file),
	line(line)
{}

std::string DefinitionLocation::toString() const
{
	return file.size() ?
		file + ':' + (line != eofLine ? std::to_string(line) : "EoF") :
		"?";
}

DefinitionLocation DefinitionLocation::createUnknown()
{
	return DefinitionLocation("", 0);
}

DefinitionLocation DefinitionLocation::createEOF(const std::string& file)
{
	return DefinitionLocation("", eofLine);
}
