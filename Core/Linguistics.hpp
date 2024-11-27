#pragma once

#include "Unit.hpp"

#include <string>

template<Unit U>
class Amount;

class Linguistics
{
public:
	static void pluralize(std::string& str);
	static std::string pluralize(const std::string& str);

	static void capitalize(std::string& str);
	static std::string capitalize(const std::string& str);

	static void formatFloatingPoint(std::string& str, const uint8_t maxDigits = 255);
	static std::string formatFloatingPoint(const std::string& str, const uint8_t maxDigits = 255);

	template<typename T, typename = std::enable_if<std::is_floating_point_v<T>>>
	static std::string formatFloatingPoint(const T& value, const uint8_t maxDigits = 255);

	static void padFront(std::string& str, const size_t minLength, const char padding);
	static std::string padFront(const std::string& str, const size_t minLength, const char padding);

	static std::string formatTime(int32_t milliseconds);

	static std::string toHex(const uint64_t value);
	static std::string toHex(const void* ptr);
};

template<typename T, typename>
std::string Linguistics::formatFloatingPoint(const T& value, const uint8_t maxDigits)
{
	return formatFloatingPoint(std::to_string(value), maxDigits);
}
