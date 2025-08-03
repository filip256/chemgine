#pragma once

#include "FileParser.hpp"
#include "FileStore.hpp"

namespace def
{
	class AnalysisResult
	{
	public:
		bool failed = true;
		size_t totalDefinitionCount = 0;
		size_t preparsedDefinitionCount = 0;
		size_t totalFileCount = 0;
		size_t preparsedFileCount = 0;
	};

	class FileAnalyzer
	{
	private:
		FileStore& mainFileStore;
		FileStore fileStore;
		def::FileParser parser;

	public:
		FileAnalyzer(
			const std::string& filePath,
			FileStore& mainFileStore
		) noexcept;
		FileAnalyzer(const FileAnalyzer&) = delete;
		FileAnalyzer(FileAnalyzer&&) = default;

		AnalysisResult analyze();
	};
}
