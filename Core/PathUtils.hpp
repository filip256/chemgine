#pragma once

#include <string>

namespace utils
{
	void normalizePath(std::string& path);
	std::string normalizePath(const std::string& path);

	std::string extractDirName(const std::string& path);
	std::string extractFileNameWithExtension(const std::string& path);
	std::string extractFileName(const std::string& path);
	std::string extractExtension(const std::string& path);

	std::string combinePaths(const std::string& path1, const std::string& path2);

	std::string getRelativePathToProjectRoot(const char* fullPath);

	bool fileExists(const std::string& path);
	void createDir(const std::string& path);
	void removeDir(const std::string& path);
}
