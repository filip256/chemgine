#pragma once

#include <string>

namespace Utils
{
	void normalizePath(std::string& path);
	std::string normalizePath(const std::string& path);

	std::string extractDirName(const std::string& path);
	std::string extractFileNameWithExtension(const std::string& path);
	std::string extractFileName(const std::string& path);
	std::string extractExtension(const std::string& path);

	std::string combinePaths(const std::string& path1, const std::string& path2);

	bool fileExists(const std::string& path);
	void createDir(const std::string& path);
	void removeDir(const std::string& path);
}
