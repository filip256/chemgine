#pragma once

#include "DefFileParser.hpp"
#include "FileStore.hpp"

class AnalysisResult
{
public:
	bool failed = true;
	size_t totalDefinitionCount = 0;
	size_t preparsedDefinitionCount = 0;
	size_t totalFileCount = 0;
	size_t preparsedFileCount = 0;
};

class DefFileAnalyzer
{
private:
	FileStore& mainFileStore;
	FileStore fileStore;
	DefFileParser parser;

public:
	DefFileAnalyzer(
		const std::string& filePath,
		FileStore& mainFileStore
	) noexcept;
	DefFileAnalyzer(const DefFileAnalyzer&) = delete;
	DefFileAnalyzer(DefFileAnalyzer&&) = default;

	AnalysisResult analyze();
};
