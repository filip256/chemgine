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
