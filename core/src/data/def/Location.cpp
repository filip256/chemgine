#include "data/def/Location.hpp"

using namespace def;

Location::Location(
	const std::string& file,
	const size_t line
) noexcept :
	file(file),
	line(line)
{}

const std::string& Location::getFile() const
{
	return file;
}

size_t Location::getLine() const
{
	return line;
}

bool Location::isEOF() const
{
	return line == eofLine;
}

std::string Location::toString() const
{
	return file.size() ?
		file + ':' + (line != eofLine ? std::to_string(line) : "EoF") :
		"?";
}

Location Location::createUnknown()
{
	return Location("", 0);
}

Location Location::createEOF(const std::string& file)
{
	return Location("", eofLine);
}
