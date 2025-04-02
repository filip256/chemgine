#include "FormatUtils.hpp"
#include "Amount.hpp"

#include <sstream>
#include <iomanip>

void Utils::pluralize(std::string& str)
{
	str += 's';
}

std::string Utils::pluralize(const std::string& str)
{
	std::string temp(str);
	pluralize(temp);
	return temp;
}

void Utils::capitalize(std::string& str)
{
	if (str.empty() || str.front() < 'a' || str.front() > 'z')
		return;

	str.front() -= ' ';
}

std::string Utils::capitalize(const std::string& str)
{
	std::string temp(str);
	capitalize(temp);
	return temp;
}

void Utils::formatFloatingPoint(std::string& str, const uint8_t maxDigits)
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

std::string Utils::formatFloatingPoint(const std::string& str, const uint8_t maxDigits)
{
	std::string temp(str);
	formatFloatingPoint(temp, maxDigits);
	return temp;
}

void Utils::padFront(std::string& str, const size_t minLenght, const char padding)
{
	const auto padLen = minLenght - str.size();
	if (minLenght <= str.size())
		return;
	
	str = std::string(minLenght - str.size(), padding) + str;
}

std::string Utils::padFront(const std::string& str, const size_t minLength, const char padding)
{
	std::string temp(str);
	padFront(temp, minLength, padding);
	return temp;
}

std::string Utils::toHex(const uint64_t value)
{
	std::stringstream ss;
	ss << std::hex << std::showbase << value;
	return ss.str();
}

std::string Utils::toHex(const void* ptr)
{
	return toHex(reinterpret_cast<uint64_t>(ptr));
}
