#include "atomics/data/AtomBaseData.hpp"

AtomBaseData::AtomBaseData(
    Symbol&&                          symbol,
    std::string&&                     name,
    const Amount<Unit::GRAM_PER_MOLE> weight,
    const AtomPrecedence              precedence) noexcept :
    symbol(std::move(symbol)),
    name(std::move(name)),
    weight(weight),
    precedence(precedence)
{}

std::string AtomBaseData::getSMILES() const
{
    const auto smiles = symbol.str();
    return smiles.size() > 1 ? '[' + smiles + ']' : smiles;
}

AtomPrecedence AtomBaseData::getPrecedence() const { return precedence; }

void AtomBaseData::print(std::ostream& out) const { dumpDefinition(out, true); }
