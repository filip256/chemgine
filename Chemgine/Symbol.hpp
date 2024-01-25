#pragma once

#include <string>

class Symbol
{
private:
	const char symbol[2] = { 0 };

public:
	Symbol(const char chr) noexcept;
	Symbol(const char* str) noexcept;
	Symbol(const std::string& str) noexcept;
	Symbol(const Symbol&) = default;

	const char* get2ByteRepr() const;

	bool operator==(const char other) const;
	bool operator!=(const char other) const;
	bool operator==(const char* other) const;
	bool operator!=(const char* other) const;
	bool operator==(const std::string& other) const;
	bool operator!=(const std::string& other) const;
	bool operator==(const Symbol& other) const;
	bool operator!=(const Symbol& other) const;

	friend std::ostream& operator<<(std::ostream& out, const Symbol& obj);

	friend struct std::hash<Symbol>;
};

template<>
struct std::hash<Symbol>
{
	size_t operator()(const Symbol& s) const noexcept
	{
		return (static_cast<size_t>(s.symbol[1]) << 8) | static_cast<size_t>(s.symbol[0]);
	}
};