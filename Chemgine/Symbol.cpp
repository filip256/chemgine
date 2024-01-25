#include "Symbol.hpp"

#include <iostream>

Symbol::Symbol(const char chr) noexcept :
	symbol{ chr }
{}

Symbol::Symbol(const char* str) noexcept :
	symbol{ str[0], str[1] }
{}

Symbol::Symbol(const std::string& str) noexcept :
	Symbol(str.c_str())
{}

const char* Symbol::get2ByteRepr() const
{
	return symbol;
}

bool Symbol::operator==(const char other) const
{
	return this->symbol[0] == other && this->symbol[1] == '\0';
}

bool Symbol::operator!=(const char other) const
{
	return !(*this == other);
}

bool Symbol::operator==(const char* other) const
{
	return this->symbol[0] == other[0] && this->symbol[1] == other[1];
}

bool Symbol::operator!=(const char* other) const
{
	return !(*this == other);
}

bool Symbol::operator==(const std::string& other) const
{
	return *this == other.c_str();
}

bool Symbol::operator!=(const std::string& other) const
{
	return *this != other.c_str();
}

bool Symbol::operator==(const Symbol& other) const
{
	return this->symbol[0] == other.symbol[0] && this->symbol[1] == other.symbol[1];
}

bool Symbol::operator!=(const Symbol& other) const
{
	return !(*this == other);
}

std::ostream& operator<<(std::ostream& out, const Symbol& obj)
{
	return obj.symbol[1] ? 
		out << obj.symbol[0] << obj.symbol[1] :
		out << obj.symbol[0];
}
