#include "data/values/SymbolMatch.hpp"

SymbolMatch::SymbolMatch(Symbol&& symbol, const bool isInferred) noexcept :
    Base(std::move(symbol)),
    isInferred(isInferred)
{}

const Symbol& SymbolMatch::getSymbol() const { return key; }

bool SymbolMatch::operator==(const SymbolMatch& other) const { return this->key == other.key; }
