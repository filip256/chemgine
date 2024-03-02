#pragma once

#include <string>

class Linguistics
{
public:
	static void pluralize(std::string& str);
	static std::string pluralize(const std::string& str);

	static void capitalize(std::string& str);
	static std::string capitalize(const std::string& str);
};
