#include "atomics/data/AtomData.hpp"

#include "data/def/DataDumper.hpp"
#include "data/def/Keywords.hpp"
#include "data/def/Object.hpp"
#include "data/def/Printers.hpp"

const ImmutableSet<uint8_t> AtomData::RadicalAnyValence = {NullValence};

AtomData::AtomData(
    const Symbol&            symbol,
    const std::string&       name,
    const Amount<Unit::GRAM> weight,
    ImmutableSet<uint8_t>&&  valences) noexcept :
    BaseComponentData(weight, getRarityOf(symbol)),
    valences(std::move(valences)),
    symbol(symbol),
    name(name)
{}

bool AtomData::isRadical() const { return weight == 0.0; }

bool AtomData::hasValence(const uint8_t v) const
{
    for (size_t i = 0; i < valences.size(); ++i)
        if (v == valences[i])
            return true;
    return false;
}

const ImmutableSet<uint8_t>& AtomData::getValences() const { return valences; }

uint8_t AtomData::getFittingValence(const uint8_t bonds) const
{
    if (valences == RadicalAnyValence)
        return bonds != 0 ? bonds : 1;

    for (size_t i = 0; i < valences.size(); ++i)
        if (bonds <= valences[i])
            return valences[i];

    return AtomData::NullValence;
}

std::string AtomData::getSMILES() const
{
    const auto smiles = symbol.str();
    return smiles.size() > 1 ? '[' + smiles + ']' : smiles;
}

void AtomData::dumpDefinition(std::ostream& out, const bool prettify) const
{
    static constexpr auto valueOffset = checked_cast<uint8_t>(
        utils::max(def::Atoms::Name.size(), def::Atoms::Weight.size(), def::Atoms::Valences.size()));

    def::DataDumper(out, valueOffset, 0, prettify)
        .header(def::Types::Atom, symbol, "")
        .beginProperties()
        .propertyWithSep(def::Atoms::Name, name)
        .propertyWithSep(def::Atoms::Weight, weight)
        .property(def::Atoms::Valences, valences)
        .endProperties()
        .endDefinition();
}

void AtomData::print(std::ostream& out) const { dumpDefinition(out, true); }

uint8_t AtomData::getRarityOf(const Symbol& symbol)
{
    static const std::unordered_map<Symbol, uint8_t> rarities{
        { "C",   1},
        { "O",   2},
        { "N",   3},
        {"Cl",   4},
        {"Br",   4},
        { "I",   5},
        { "F",   6},
        { "S",   7},
        { "P",   8},
        { "B",   9},
        {"Al",  10},
        {"Se",  11},
        { "H", 100},
    };

    const auto r = rarities.find(symbol);
    return r != rarities.end() ? r->second : 255;
}
