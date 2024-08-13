#pragma once

#include <string>

class DefinitionLocation
{
private:
	const static size_t eofLine = static_cast<size_t>(-1);

	std::string file;
	size_t line;

public:
	DefinitionLocation(
		const std::string& file,
		const size_t line
	) noexcept;
	DefinitionLocation(const DefinitionLocation&) = delete;
	DefinitionLocation(DefinitionLocation&&) = default;

	DefinitionLocation& operator=(DefinitionLocation&&) = default;

	const std::string& getFile() const;
	size_t getLine() const;

	std::string toString() const;

	static DefinitionLocation createUnknown();
	static DefinitionLocation createEOF(const std::string& file);
};
