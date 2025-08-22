#pragma once

#include "data/def/Object.hpp"

#include <fstream>
#include <optional>

class FileStore;
class OOLDefRepository;

namespace def
{

class FileParser
{
private:
    size_t                                       currentLine = 0;
    std::string                                  currentFile;
    std::ifstream                                stream;
    std::unique_ptr<FileParser>                  subParser = nullptr;
    std::unordered_map<std::string, std::string> includeAliases;
    const OOLDefRepository&                      oolDefinitions;

    FileStore& fileStore;

    void include(const std::string& filePath);
    void closeSubparser();

public:
    FileParser(
        const std::string&      filePath,
        FileStore&              fileStore,
        const OOLDefRepository& oolDefinitions) noexcept;
    FileParser(const FileParser&) = delete;
    FileParser(FileParser&&)      = default;
    ~FileParser() noexcept;

    /// <summary>
    /// Returns the status of the current stream.
    /// </summary>
    bool isOpen() const;

    /// <summary>
    /// Closes the current stream and sets the parse status as completed.
    /// </summary>
    void forceFinish();

    /// <summary>
    /// Returns the location of the last returned line in the current
    /// definition file.
    /// </summary>
    def::Location getCurrentLocalLocation() const;

    /// <summary>
    /// Returns the location of the last returned line of the currently
    /// parsed definition file, taking into account included files.
    /// </summary>
    def::Location getCurrentGlobalLocation() const;

    /// <summary>
    /// Returns the next non-empty line of the current definition file or
    /// "" if EOF was reached.
    /// </summary>
    std::string nextLocalLine();

    /// <summary>
    /// Returns the next non-empty line of the currently parsed definition
    /// file, taking into account included files, or "" if EOF was reached.
    /// </summary>
    std::string nextGlobalLine();

    /// <summary>
    /// Returns the next complete definition string, taking into account
    /// included files, together with the location where the definition
    /// started.
    /// </summary>
    std::pair<std::string, def::Location> nextDefinitionLine();

    /// <summary>
    /// Returns the next parsed def::Object if parsing succeeds and
    /// std::nullopt otherwise.
    /// </summary>
    std::optional<def::Object> nextDefinition();
};

}  // namespace def
