#include "Symbol.hpp"
#include "Log.hpp"

#include <iostream>

Symbol::Symbol(const std::string& str) noexcept :
	symbol(str.c_str())
{}

Symbol::Symbol(std::string&& str) noexcept :
	symbol(std::move(str))
{}

Symbol::Symbol(const char chr) noexcept :
	symbol(std::string(1, chr))
{}

Symbol::Symbol(const char* str) noexcept :
	Symbol(std::string(str))
{}

const std::string& Symbol::getString() const
{
	return symbol;
}

bool Symbol::operator==(const Symbol& other) const
{
	return this->symbol == other.symbol;
}

bool Symbol::operator!=(const Symbol& other) const
{
	return this->symbol != other.symbol;
}

bool Symbol::operator==(const std::string& other) const
{
	return this->symbol == other;
}

bool Symbol::operator!=(const std::string& other) const
{
	return this->symbol != other;
}

std::ostream& operator<<(std::ostream& out, const Symbol& obj)
{
	return out << obj.symbol;
}
