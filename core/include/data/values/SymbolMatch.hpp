#pragma once

#include "data/def/Printers.hpp"
#include "data/values/Symbol.hpp"
#include "structs/KeyValueObject.hpp"

class SymbolMatch : public KeyValueObject<Symbol>
{
    using Base = KeyValueObject<Symbol>;

public:
    bool isInferred;

    SymbolMatch(Symbol&& symbol, const bool isInferred) noexcept;

    const Symbol& getSymbol() const;

    bool operator==(const SymbolMatch& other) const;

    using Set = Base::UnorderedSet<SymbolMatch>;
};

using SymbolMatchSet = SymbolMatch::Set;

//
// Extras
//

template <>
class def::Printer<SymbolMatch>
{
public:
    static std::string print(const SymbolMatch& object) { return def::print(object.getSymbol()); }
};
