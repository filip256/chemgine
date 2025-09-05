#include "data/def/FileAnalyzer.hpp"

#include "data/OutlineDefRepository.hpp"
#include "data/def/Keywords.hpp"

using namespace def;

FileAnalyzer::FileAnalyzer(const std::string& filePath, FileStore& mainFileStore) noexcept :
    mainFileStore(mainFileStore),
    parser(def::FileParser(filePath, fileStore, {}))
{}

AnalysisResult FileAnalyzer::analyze()
{
    LogBase::hide();

    AnalysisResult result;
    if (parser.isOpen() == false)
        return result;

    while (true) {
        const auto line     = parser.nextGlobalLine();
        const auto location = parser.getCurrentGlobalLocation();
        if (parser.isOpen() == false)
            break;

        if (line.starts_with('_')) {
            ++result.totalDefinitionCount;
            if (mainFileStore.getFileStatus(location.getFile()) == ParseStatus::COMPLETED)
                ++result.preparsedDefinitionCount;
        }
    }

    const auto& fileHistory = fileStore.getHistory();
    result.totalFileCount   = fileHistory.size();
    for (const auto& f : fileHistory)
        if (mainFileStore.getFileStatus(f.first) == ParseStatus::COMPLETED)
            ++result.preparsedFileCount;

    result.failed = false;

    LogBase::unhide();
    return result;
}
