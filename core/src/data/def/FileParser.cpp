#include "data/def/FileParser.hpp"

#include "data/FileStore.hpp"
#include "data/OutlineDefRepository.hpp"
#include "data/def/DefinitionParser.hpp"
#include "data/def/Keywords.hpp"
#include "data/def/Parsers.hpp"
#include "io/Log.hpp"
#include "utils/Path.hpp"
#include "utils/String.hpp"

using namespace def;

FileParser::FileParser(
    const std::string& filePath, FileStore& fileStore, const OutlineDefRepository& outlineDefinitions) noexcept :
    currentFile(utils::normalizePath(filePath)),
    stream(currentFile),
    outlineDefinitions(outlineDefinitions),
    fileStore(fileStore)
{
    if (not stream.is_open()) {
        Log(this).error("Failed to open file: '{}' for reading.", currentFile);
        return;
    }

    if (fileStore.getFileStatus(filePath) == ParseStatus::COMPLETED) {
        Log(this).warn("Skipping already loaded file: '{}'.", filePath);
        forceFinish();
        return;
    }

    fileStore.setFileStatus(currentFile, ParseStatus::STARTED);
}

FileParser::~FileParser() noexcept
{
    if (stream.is_open()) {
        Log(this).warn("Incomplete parsing on file: '{}'.", currentFile);
        stream.close();
    }
}

void FileParser::include(const std::string& filePath)
{
    const auto status = fileStore.getFileStatus(filePath);
    if (status == ParseStatus::COMPLETED)  // skip already parsed file
        return;

    if (status == ParseStatus::STARTED) {
        Log(this).error("Encountered cyclic dependency on file: '{}', at: {}:{}.", filePath, currentFile, currentLine);
        return;
    }

    subParser = std::make_unique<FileParser>(filePath, fileStore, outlineDefinitions);
}

void FileParser::closeSubparser()
{
    // inherit include aliases
    includeAliases.merge(subParser->includeAliases);
    for (const auto& a : subParser->includeAliases)
        if (includeAliases[a.first] != a.second)
            Log(this).warn(
                "Overwritten already defined include alias: '{}: {}' from included file, at: "
                "{}:{}.",
                a.first,
                a.second,
                currentFile,
                currentLine);

    subParser->stream.close();
    subParser.reset(nullptr);
}

bool FileParser::isOpen() const { return stream.is_open(); }

def::Location FileParser::getCurrentLocalLocation() const
{
    return isOpen() ? def::Location(currentFile, currentLine) : def::Location::createEOF(currentFile);
}

def::Location FileParser::getCurrentGlobalLocation() const
{
    return subParser ? subParser->getCurrentGlobalLocation() : getCurrentLocalLocation();
}

void FileParser::forceFinish()
{
    if (isOpen()) {
        fileStore.setFileStatus(currentFile, ParseStatus::COMPLETED);
        stream.close();
    }
}

std::string FileParser::nextLocalLine()
{
    std::string line;
    while (std::getline(stream, line)) {
        ++currentLine;

        utils::strip(line);
        if (line.empty())
            continue;

        return line;
    }

    forceFinish();
    return "";
}

std::string FileParser::nextGlobalLine()
{
    // finish includes first
    if (subParser) {
        const auto subLine = subParser->nextGlobalLine();
        if (subParser->isOpen())
            return subLine;

        closeSubparser();
    }

    // main file
    while (true) {
        const auto line = nextLocalLine();
        if (line.empty())
            break;

        // include
        if (line.starts_with(def::Syntax::Include)) {
            const auto pathEnd = line.find(def::Syntax::IncludeAs, def::Syntax::Include.size());
            auto       path =
                utils::normalizePath(line.substr(def::Syntax::Include.size(), pathEnd - def::Syntax::Include.size()));

            // append dir
            if (path.starts_with("~/"))
                path = utils::combinePaths(utils::extractDirName(currentFile), path.substr(1));

            include(path);
            return nextGlobalLine();
        }

        return line;
    }

    forceFinish();
    return "";
}

std::pair<std::string, def::Location> FileParser::nextDefinitionLine()
{
    // finish includes first
    if (subParser) {
        auto subLine = subParser->nextDefinitionLine();
        if (subLine.first.size())
            return subLine;

        closeSubparser();
    }

    // main file
    while (true) {
        auto line = nextLocalLine();
        if (line.empty())
            break;

        // single-line comment
        if (line.starts_with("::"))
            continue;

        // multi-line comment
        if (line.starts_with(":.")) {
            bool commentClosed = false;
            while (true) {
                line = nextLocalLine();
                if (line.empty())
                    break;

                if (line.ends_with(".:")) {
                    commentClosed = true;
                    break;
                }
            };

            if (commentClosed)
                continue;

            Log(this).error("Missing multi-line definition terminator: '.:', at: {}:{}.", currentFile, currentLine);
        }

        // debug message
        if (line.starts_with(">>")) {
            line = line.substr(2);
            utils::strip(line);
            Log(this).debug("{}", line);
            continue;
        }

        // include
        if (line.starts_with(def::Syntax::Include)) {
            const auto pathEnd = line.find(def::Syntax::IncludeAs, def::Syntax::Include.size());
            auto       path =
                utils::normalizePath(line.substr(def::Syntax::Include.size(), pathEnd - def::Syntax::Include.size()));

            // append dir
            if (path.starts_with("~/"))
                path = utils::combinePaths(utils::extractDirName(currentFile), path.substr(1));

            if (path.empty()) {
                Log(this).error(
                    "Missing include path after '{}' keyword, at: {}:{}.",
                    def::Syntax::Include,
                    currentFile,
                    currentLine);
                continue;
            }

            if (pathEnd != std::string::npos) {
                auto alias = utils::strip(line.substr(pathEnd + def::Syntax::IncludeAs.size()));
                if (alias.empty()) {
                    Log(this).error(
                        "Missing include alias after '{}' keyword, at: {}:{}.",
                        def::Syntax::IncludeAs,
                        currentFile,
                        currentLine);
                    continue;
                }

                if (auto status = includeAliases.emplace(std::move(alias), path); not status.second) {
                    Log(this).warn(
                        "Redefinition of an existing include alias: '{}: {}', at: {}:{}.",
                        alias,
                        status.first->second,
                        currentFile,
                        currentLine);
                    status.first->second = path;
                }
            }

            include(path);
            return nextDefinitionLine();
        }

        // defs
        if (line.starts_with('_')) {
            auto location = getCurrentLocalLocation();

            // single-line def
            if (line.ends_with(';'))
                return std::make_pair(line, std::move(location));

            // multi-line def
            while (true) {
                const auto newLine = nextLocalLine();
                if (newLine.empty())
                    break;

                // skip comments
                if (newLine.starts_with("::"))
                    continue;

                line += ' ' + newLine;
                if (newLine.ends_with(';'))
                    return std::make_pair(line, std::move(location));
            };

            Log(this).error("Missing definition terminator: ';', at: {}.", location.toString());
            continue;
        }

        Log(this).error("Unknown synthax: '{}', at: {}:{}.", line, currentFile, currentLine);
    };

    forceFinish();
    return std::make_pair("", def::Location::createEOF(currentFile));
}

std::optional<def::Object> FileParser::nextDefinition()
{
    // finish includes first
    if (subParser) {
        auto subDef = subParser->nextDefinition();
        if (subDef)
            return subDef;

        // continue parsing until EoF, even if error occurs
        if (not subParser->isOpen())
            closeSubparser();
    }

    auto [line, location] = nextDefinitionLine();
    if (location.isEOF())
        return std::nullopt;

    // TODO: remove dirty trick to pass subparser's include aliases to this's parser (needed for the
    // first def in a file)
    return def::parse<def::Object>(
        line, std::move(location), subParser ? subParser->includeAliases : includeAliases, outlineDefinitions);
}
