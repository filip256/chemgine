#include "Linguistics.hpp"

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
	formatFloatingPoint(temp);
	return temp;
}
