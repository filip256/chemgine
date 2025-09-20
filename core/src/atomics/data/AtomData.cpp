#include "atomics/data/AtomData.hpp"

#include "data/def/DataDumper.hpp"
#include "data/def/Keywords.hpp"
#include "data/def/Object.hpp"
#include "data/def/Printers.hpp"

namespace
{

uint8_t getPrecedenceOf(const Symbol& symbol)
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

}  // namespace

AtomData::AtomData(
    Symbol&&                          symbol,
    std::string&&                     name,
    const Amount<Unit::GRAM_PER_MOLE> weight,
    ImmutableSet<uint8_t>&&           valences) noexcept :
    AtomBaseData(std::move(symbol), std::move(name), weight),
    valences(std::move(valences)),
    rarity(getPrecedenceOf(this->symbol))
{}

bool AtomData::isRadical() const { return false; }

const ImmutableSet<uint8_t>& AtomData::getValences() const { return valences; }

uint8_t AtomData::getFittingValence(const uint8_t bonds) const
{
    const auto vIt = std::ranges::find_if(valences, [bonds](const uint8_t v) { return v >= bonds; });
    return vIt != valences.end() ? *vIt : NullValence;
}

bool AtomData::hasValence(const uint8_t valence) const
{
    // Usually an atom has less than 8 valences, linear search if preferred.
    return std::ranges::any_of(getValences(), [valence](const uint8_t v) { return v == valence; });
}

uint8_t AtomData::getPrecedence() const { return rarity; }

void AtomData::dumpDefinition(std::ostream& out, const bool prettify) const
{
    static constexpr auto valueOffset = checked_cast<uint8_t>(
        utils::max(def::Atoms::Name.size(), def::Atoms::Weight.size(), def::Atoms::Valences.size()));

    def::DataDumper(out, valueOffset, 0, prettify)
        .header(def::Types::Atom, symbol, "")
        .beginProperties()
        .property(def::Atoms::Name, name)
        .property(def::Atoms::Weight, weight)
        .property(def::Atoms::Valences, valences)
        .endProperties()
        .endDefinition();
}
