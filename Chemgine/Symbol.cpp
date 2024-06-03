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

bool Symbol::isSingleByte() const
{
	return symbol[1] == '\0';
}

const char* Symbol::get2ByteRepr() const
{
	return symbol;
}

std::string Symbol::getAsString() const
{
	return isSingleByte() ?
		std::string({ symbol[0] }) :
		std::string({ symbol[0], symbol[1] });
}

bool Symbol::operator==(const char other) const
{
	return this->symbol[0] == other && isSingleByte();
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

bool Symbol::operator==(const Symbol other) const
{
	return this->symbol[0] == other.symbol[0] && this->symbol[1] == other.symbol[1];
}

bool Symbol::operator!=(const Symbol other) const
{
	return !(*this == other);
}

std::ostream& operator<<(std::ostream& out, const Symbol obj)
{
	return obj.isSingleByte() ?
		out << obj.symbol[0] :
		out << obj.symbol[0] << obj.symbol[1];
}
