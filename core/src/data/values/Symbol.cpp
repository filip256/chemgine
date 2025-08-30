#include "data/values/Symbol.hpp"

#include "io/Log.hpp"

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

const std::string& Symbol::str() const { return symbol; }

const Symbol::SizeT Symbol::size() const { return static_cast<SizeT>(symbol.size()); }

bool Symbol::operator==(const Symbol& other) const { return this->symbol == other.symbol; }

bool Symbol::operator!=(const Symbol& other) const { return this->symbol != other.symbol; }

bool Symbol::operator==(const std::string& other) const { return this->symbol == other; }

bool Symbol::operator!=(const std::string& other) const { return this->symbol != other; }

std::ostream& operator<<(std::ostream& out, const Symbol& obj) { return out << obj.symbol; }
