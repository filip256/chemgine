#pragma once

#include "DefinitionObject.hpp"

#include <fstream>
#include <optional>

class FileStore;

class DefFileParser
{
private:
	size_t currentLine = 0;
	std::string currentFile;
	std::ifstream stream;
	std::unique_ptr<DefFileParser> subParser = nullptr;
	std::unordered_map<std::string, std::string> includeAliases;

	FileStore& fileStore;

	void include(const std::string& filePath);
	
public:
	DefFileParser(
		const std::string& filePath,
		FileStore& fileStore
	) noexcept;
	DefFileParser(const DefFileParser&) = delete;
	DefFileParser(DefFileParser&&) = default;
	~DefFileParser() noexcept;

	bool isOpen() const;

	DefinitionLocation getCurrentLocalLocation() const;
	DefinitionLocation getCurrentGlobalLocation() const;

	void forceFinish();
	std::string nextLocalLine();
	std::string nextGlobalLine();
	std::pair<std::string, DefinitionLocation> nextDefinitionLine();
	std::optional<DefinitionObject> nextDefinition();
};
