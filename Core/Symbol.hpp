#pragma once

#include "Parsers.hpp"
#include "Printers.hpp"

#include <string>

class Symbol
{
private:
	const std::string symbol;

public:
	Symbol(const std::string& str) noexcept;
	Symbol(std::string&& str) noexcept;
	Symbol(const char chr) noexcept;
	Symbol(const char* str) noexcept;

	Symbol(const Symbol&) = default;
	Symbol(Symbol&&) = default;

	const std::string& getString() const;

	bool operator==(const Symbol& other) const;
	bool operator!=(const Symbol& other) const;
	bool operator==(const std::string& other) const;
	bool operator!=(const std::string& other) const;

	friend std::ostream& operator<<(std::ostream& out, const Symbol& obj);

	friend struct std::hash<Symbol>;
};

template<>
struct std::hash<Symbol>
{
	size_t operator()(const Symbol& s) const noexcept
	{
		return std::hash<std::string>()(s.symbol);
	}
};

template <>
class Def::Parser<Symbol>
{
public:
	static std::optional<Symbol> parse(const std::string& str)
	{
		const auto stripped = Utils::strip(str);
		return Symbol(stripped);
	}
};

template <>
class Def::Printer<Symbol>
{
public:
	static std::string print(const Symbol& object)
	{
		return object.getString();
	}
};
