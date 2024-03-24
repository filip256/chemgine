#pragma once

#include <string>

class Linguistics
{
public:
	static void pluralize(std::string& str);
	static std::string pluralize(const std::string& str);

	static void capitalize(std::string& str);
	static std::string capitalize(const std::string& str);

	static void formatFloatingPoint(std::string& str, const uint8_t maxDigits = 255);
	static std::string formatFloatingPoint(const std::string& str, const uint8_t maxDigit = 255);
};
