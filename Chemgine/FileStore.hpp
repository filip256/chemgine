#pragma once

#include <string>
#include <unordered_map>

enum class ParseStatus : uint8_t
{
	UNTOUCHED,
	STARTED,
	COMPLETED
};

/// <summary>
/// Tracks the parsing status of files, helping to detect cyclic dependencies
/// and already parsed files. 
/// </summary>
class FileStore
{
private:
	std::unordered_map<std::string, bool> parseHistory;

public:
	FileStore() = default;
	FileStore(const FileStore&) = delete;
	FileStore(FileStore&&) = default;

	const std::unordered_map<std::string, bool>& getHistory() const;

	void clear();

	ParseStatus getFileStatus(const std::string& filePath) const;
	void setFileStatus(const std::string& filePath, const ParseStatus status);
};
