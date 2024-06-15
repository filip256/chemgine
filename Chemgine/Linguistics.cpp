#include "Linguistics.hpp"
#include "Amount.hpp"

void Linguistics::pluralize(std::string& str)
{
	str += 's';
}

std::string Linguistics::pluralize(const std::string& str)
{
	std::string temp(str);
	pluralize(temp);
	return temp;
}

void Linguistics::capitalize(std::string& str)
{
	if (str.empty() || str.front() < 'a' || str.front() > 'z')
		return;

	str.front() -= ' ';
}

std::string Linguistics::capitalize(const std::string& str)
{
	std::string temp(str);
	capitalize(temp);
	return temp;
}

void Linguistics::formatFloatingPoint(std::string& str, const uint8_t maxDigits)
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

std::string Linguistics::formatFloatingPoint(const std::string& str, const uint8_t maxDigit)
{
	std::string temp(str);
	formatFloatingPoint(temp, maxDigit);
	return temp;
}

void Linguistics::padFront(std::string& str, const size_t minLenght, const char padding)
{
	const auto padLen = minLenght - str.size();
	if (minLenght <= str.size())
		return;
	
	str = std::string(minLenght - str.size(), padding) + str;
}

std::string Linguistics::padFront(const std::string& str, const size_t minLength, const char padding)
{
	std::string temp(str);
	padFront(temp, minLength, padding);
	return temp;
}

std::string Linguistics::formatTime(int32_t milliseconds)
{
	const auto h = milliseconds / 3600000;
	milliseconds -= h * 3600000;
	const auto m = milliseconds / 60000;
	milliseconds -= m * 60000;
	const auto s = milliseconds / 1000;
	milliseconds -= s * 1000;

	return std::to_string(h) + ':' +
		padFront(std::to_string(m), 2, '0') + ':' +
		padFront(std::to_string(s), 2, '0') + '.' +
		std::to_string(milliseconds);
}
