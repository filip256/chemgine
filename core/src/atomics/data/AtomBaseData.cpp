#include "atomics/data/AtomBaseData.hpp"

AtomBaseData::AtomBaseData(Symbol&& symbol, std::string&& name) noexcept :
    symbol(std::move(symbol)),
    name(std::move(name))
{}

std::string AtomBaseData::getSMILES() const
{
    const auto smiles = symbol.str();
    return smiles.size() > 1 ? '[' + smiles + ']' : smiles;
}

void AtomBaseData::print(std::ostream& out) const { dumpDefinition(out, true); }
