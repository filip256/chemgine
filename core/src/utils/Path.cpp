#include "utils/String.hpp"
#include "utils/Path.hpp"
#include "io/Log.hpp"

#include <algorithm>
#include <filesystem>
#include <cstring>

void utils::normalizePath(std::string& path)
{
	utils::strip(path);
	std::replace(path.begin(), path.end(), '\\', '/');
}

std::string utils::normalizePath(const std::string& path)
{
	auto temp = path;
	normalizePath(temp);
	return temp;
}

std::string utils::extractDirName(const std::string& path)
{
	const auto dirEnd = path.rfind('/');
	return dirEnd != std::string::npos ?
		path.substr(0, dirEnd) :
		"";
}

std::string utils::extractFileNameWithExtension(const std::string& path)
{
	const auto nameStart = path.rfind('/');
	return nameStart != std::string::npos ?
		path.substr(nameStart + 1) :
		path;
}

std::string utils::extractFileName(const std::string& path)
{
	const auto nameWithExt = extractFileNameWithExtension(path);
	const auto nameEnd = nameWithExt.rfind('.');
	return nameEnd != std::string::npos ?
		nameWithExt.substr(0, nameEnd) :
		nameWithExt;
}

std::string utils::extractExtension(const std::string& path)
{
	const auto extStart = path.rfind('.');
	return extStart != std::string::npos ?
		path.substr(extStart + 1) :
		"";
}

std::string utils::combinePaths(const std::string& path1, const std::string& path2)
{
	if (path1.ends_with('/'))
	{
		return path2.starts_with('/') ?
			path1.substr(0, path1.size() - 1) + path2 :
			path1 + path2;
	}

	return path2.starts_with('/') ?
		path1 + path2 :
		path1 + '/' + path2;
}

std::string utils::getRelativePathToProjectRoot(const char* fullPath)
{
	constexpr std::string_view basePath = "Chemgine";
	const char* pathBegin = std::strstr(fullPath, basePath.data());
	return std::string(pathBegin != nullptr ? pathBegin : fullPath);
}

bool utils::fileExists(const std::string& path)
{
	return std::filesystem::exists(path);
}

void utils::createDir(const std::string& path)
{
	try
	{
		if (not std::filesystem::exists(path))
			std::filesystem::create_directory(path);
	}
	catch (const std::filesystem::filesystem_error& e)
	{
		Log().fatal("Failed to create directory: '{0}'\n'{1}'", path, e.what());
	}
}

void utils::removeDir(const std::string& path)
{
	try
	{
		if (std::filesystem::exists(path))
			std::filesystem::remove_all(path);
	}
	catch (const std::filesystem::filesystem_error& e)
	{
		Log().fatal("Failed to remove directory: '{0}'\n'{1}'", path, e.what());
	}
}
