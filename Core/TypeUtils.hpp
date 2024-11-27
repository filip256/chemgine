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
	if (name.starts_with("class"))
		name = name.substr(5);

	Utils::strip(name);
	return name;
}
