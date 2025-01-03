#pragma once

#include "StringUtils.hpp"

#include <string>

namespace Utils
{
	template<typename T>
	std::string getTypeName();
}

template<typename T>
std::string Utils::getTypeName()
{
	if constexpr (std::is_same<T, void>())
		return "";

	auto name = std::string(typeid(T).name());

	const std::string classStr = "class ";
	const std::string structStr = "struct ";

	size_t pos;
	while ((pos = name.find(classStr)) != std::string::npos)
		name.erase(pos, classStr.size());
	while ((pos = name.find(structStr)) != std::string::npos)
		name.erase(pos, structStr.size());

	Utils::strip(name);
	return name;
}
