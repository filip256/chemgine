#pragma once

#include "data/def/Parsers.hpp"
#include "data/def/Printers.hpp"

#include <string>

class Symbol
{
private:
    const std::string symbol;

public:
    using SizeT = uint8_t;

    Symbol(const std::string& str) noexcept;
    Symbol(std::string&& str) noexcept;
    Symbol(const char chr) noexcept;
    Symbol(const char* str) noexcept;

    Symbol(const Symbol&) = default;
    Symbol(Symbol&&)      = default;

    const std::string& str() const;

    const SizeT size() const;

    bool operator==(const Symbol& other) const;
    bool operator!=(const Symbol& other) const;
    bool operator==(const std::string& other) const;
    bool operator!=(const std::string& other) const;

    friend std::ostream& operator<<(std::ostream& out, const Symbol& obj);

    friend struct std::hash<Symbol>;
};

//
// Extras
//

template <>
struct std::hash<Symbol>
{
    size_t operator()(const Symbol& s) const noexcept { return std::hash<std::string>()(s.symbol); }
};

template <>
class def::Parser<Symbol>
{
public:
    static std::optional<Symbol> parse(const std::string& str)
    {
        const auto stripped = utils::strip(str);
        return Symbol(stripped);
    }
};

template <>
class def::Printer<Symbol>
{
public:
    static std::string print(const Symbol& object) { return object.str(); }
};

template <>
struct std::formatter<Symbol>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const Symbol& symbol, FormatContext& ctx) const
    {
        return format_to(ctx.out(), "{}", symbol.str());
    }
};
