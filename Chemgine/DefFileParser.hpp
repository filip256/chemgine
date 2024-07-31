#pragma once

#include "DefinitionObject.hpp"

#include <fstream>
#include <optional>

class DataStore;

class DefFileParser
{
private:
	size_t currentLine = 0;
	std::string currentFile;
	std::ifstream stream;
	std::unique_ptr<DefFileParser> subParser = nullptr;

	DataStore& dataStore;

	void include(const std::string& filePath);
	
public:
	DefFileParser(
		const std::string& filePath,
		DataStore& dataStore
	) noexcept;
	DefFileParser(DefFileParser&&) = default;
	~DefFileParser() noexcept;

	bool isOpen() const;

	void forceFinish();
	std::string nextLine();
	std::pair<std::string, DefinitionLocation> nextDefinitionLine();
	std::optional<DefinitionObject> nextDefinition();
};
