#include "StringUtils.hpp"
#include "PathUtils.hpp"
#include "Log.hpp"

#include <algorithm>
#include <filesystem>

void Utils::normalizePath(std::string& path)
{
	Utils::strip(path);
	std::replace(path.begin(), path.end(), '\\', '/');
}

std::string Utils::normalizePath(const std::string& path)
{
	auto temp = path;
	normalizePath(temp);
	return temp;
}

std::string Utils::extractDirName(const std::string& path)
{
	const auto dirEnd = path.rfind('/');
	return dirEnd != std::string::npos ?
		path.substr(0, dirEnd) :
		"";
}

std::string Utils::extractFileNameWithExtension(const std::string& path)
{
	const auto nameStart = path.rfind('/');
	return nameStart != std::string::npos ?
		path.substr(nameStart + 1) :
		path;
}

std::string Utils::extractFileName(const std::string& path)
{
	const auto nameWithExt = extractFileNameWithExtension(path);
	const auto nameEnd = nameWithExt.rfind('.');
	return nameEnd != std::string::npos ?
		nameWithExt.substr(0, nameEnd) :
		nameWithExt;
}

std::string Utils::extractExtension(const std::string& path)
{
	const auto extStart = path.rfind('.');
	return extStart != std::string::npos ?
		path.substr(extStart + 1) :
		"";
}

std::string Utils::combinePaths(const std::string& path1, const std::string& path2)
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

bool Utils::fileExists(const std::string& path)
{
	return std::filesystem::exists(path);
}


void Utils::createDir(const std::string& path)
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


void Utils::removeDir(const std::string& path)
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
