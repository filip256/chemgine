#include "FileStore.hpp"

const std::unordered_map<std::string, bool>& FileStore::getHistory() const
{
	return parseHistory;
}

ParseStatus FileStore::getFileStatus(const std::string& filePath) const
{
	const auto it = parseHistory.find(filePath);
	return it == parseHistory.end() ? ParseStatus::UNTOUCHED :
		it->second ? ParseStatus::PARSED :
		ParseStatus::STARTED;
}

void FileStore::setFileStatus(const std::string& filePath, const ParseStatus status)
{
	const auto it = parseHistory.find(filePath);
	if (it == parseHistory.end())
	{
		if(status != ParseStatus::UNTOUCHED)
			parseHistory.emplace(filePath, status == ParseStatus::PARSED);
	}
	else if (status == ParseStatus::UNTOUCHED)
		parseHistory.erase(it);
	else
		it->second = (status == ParseStatus::PARSED);
}