#include "StringUtils.hpp"
#include "PathUtils.hpp"

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
