#include "DefFileAnalyzer.hpp"
#include "Keywords.hpp"

using namespace Keywords;

DefFileAnalyzer::DefFileAnalyzer(
	const std::string& filePath,
	FileStore& mainFileStore
) noexcept:
	mainFileStore(mainFileStore),
	parser(DefFileParser(filePath, fileStore))
{}

AnalysisResult DefFileAnalyzer::analyze()
{
	const auto logLevel = LogBase::logLevel;
	LogBase::logLevel = LogType::NONE;

	AnalysisResult result;
	if (parser.isOpen() == false)
		return result;

	while (true)
	{
		const auto line = parser.nextGlobalLine();
		const auto location = parser.getCurrentGlobalLocation();
		if (parser.isOpen() == false)
			break;

		if (line.starts_with('_'))
		{
			++result.totalDefinitionCount;
			if (mainFileStore.getFileStatus(location.getFile()) == ParseStatus::PARSED)
				++result.preparsedDefinitionCount;
		}
	}

	const auto& fileHistory = fileStore.getHistory();
	result.totalFileCount = fileHistory.size();
	for (const auto& f : fileHistory)
		if (mainFileStore.getFileStatus(f.first) == ParseStatus::PARSED)
			++result.preparsedFileCount;

	result.failed = false;

	LogBase::logLevel = logLevel;
	return result;
}