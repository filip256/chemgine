#include "utils/Format.hpp"
#include "data/values/Amount.hpp"

#include <sstream>
#include <iomanip>

void utils::pluralize(std::string& str)
{
	str += 's';
}

std::string utils::pluralize(const std::string& str)
{
	std::string temp(str);
	pluralize(temp);
	return temp;
}

void utils::capitalize(std::string& str)
{
	if (str.empty() || str.front() < 'a' || str.front() > 'z')
		return;

	str.front() -= ' ';
}

std::string utils::capitalize(const std::string& str)
{
	std::string temp(str);
	capitalize(temp);
	return temp;
}

void utils::formatFloatingPoint(std::string& str, const uint8_t maxDigits)
{
	str = str.substr(0, maxDigits);

	const auto p = str.find('.');
	if (p >= str.size() - 2)
		return;

	const auto z = str.find_last_not_of('0');
	if (z <= p)
	{
		str = str.substr(0, p + 2);
		return;
	}

	str = str.substr(0, z + 1);
}

std::string utils::formatFloatingPoint(const std::string& str, const uint8_t maxDigits)
{
	std::string temp(str);
	formatFloatingPoint(temp, maxDigits);
	return temp;
}

void utils::padLeft(std::string& str, const size_t minLength, const char padding)
{
	if (minLength > str.size())
		str = std::string(minLength - str.size(), padding) + str;
}

std::string utils::padLeft(const std::string& str, const size_t minLength, const char padding)
{
	return minLength > str.size() ? std::string(minLength - str.size(), padding) + str : str;
}

void utils::padRight(std::string& str, const size_t minLength, const char padding)
{
	if (minLength > str.size())
		str += std::string(minLength - str.size(), padding);
}

std::string utils::padRight(const std::string& str, const size_t minLength, const char padding)
{
	return minLength > str.size() ? str + std::string(minLength - str.size(), padding) : str;
}

std::string utils::toHex(const uint64_t value)
{
	std::stringstream ss;
	ss << std::hex << std::showbase << value;
	return ss.str();
}

std::string utils::toHex(const void* ptr)
{
	return toHex(reinterpret_cast<uint64_t>(ptr));
}
