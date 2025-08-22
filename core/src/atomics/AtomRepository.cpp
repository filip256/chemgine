#include "atomics/AtomRepository.hpp"

#include "data/def/Keywords.hpp"
#include "data/values/DynamicAmount.hpp"
#include "io/Log.hpp"
#include "utils/Build.hpp"
#include "utils/STL.hpp"

#include <fstream>

template <>
bool AtomRepository::add<AtomData>(const def::Object& definition)
{
    const auto symbol = def::parse<Symbol>(definition.getSpecifier());
    if (not symbol) {
        Log(this).error(
            "Invalid atom symbol: '{0}' at: {1}.",
            definition.getSpecifier(),
            definition.getLocationName());
        return false;
    }

    const auto name   = definition.getProperty(def::Atoms::Name);
    const auto weight = definition.getProperty(def::Atoms::Weight, def::parse<Amount<Unit::GRAM>>);
    auto valences = definition.getProperty(def::Atoms::Valences, def::parse<std::vector<uint8_t>>);

    if (not(name && weight && valences)) {
        Log(this).error("Incomplete atom definition at: {0}.", definition.getLocationName());
        return false;
    }

    if (not atoms
                .emplace(
                    *symbol,
                    std::make_unique<AtomData>(*symbol, *name, *weight, std::move(*valences)))
                .second) {
        Log(this).warn("Atom with duplicate symbol: '{0}' skipped.", *symbol);
        return false;
    }

    definition.logUnusedWarnings();
    return true;
}

template <>
bool AtomRepository::add<RadicalData>(const def::Object& definition)
{
    const auto symbol = def::parse<Symbol>(definition.getSpecifier());
    if (not symbol) {
        Log(this).error(
            "Invalid radical symbol: '{0}' at: {1}.",
            definition.getSpecifier(),
            definition.getLocationName());
        return false;
    }

    const auto name = definition.getDefaultProperty(def::Atoms::Name, utils::copy(symbol->str()));
    const auto matches =
        definition.getProperty(def::Atoms::RadicalMatches, def::parse<std::vector<Symbol>>);

    if (not matches) {
        Log(this).error("Incomplete radical definition at: {0}.", definition.getLocationName());
        return false;
    }

    // check match symbols
    for (size_t i = 0; i < matches->size(); ++i) {
        const auto& symbol = (*matches)[i];
        if (symbol == '*')  // match-any symbol
        {
            if (matches->size() != 1)
                Log(this).warn(
                    "Found redundant atom match symbols in set containing the match-any symbol: "
                    "'*', at: {0}.",
                    definition.getLocationName());
            continue;
        }

        if (contains(symbol) == false) {
            Log(this).error(
                "Unknown atom match symbol: '{0}', at: {1}.", symbol, definition.getLocationName());
            return false;
        }
    }

    std::unordered_set matchSet(matches->begin(), matches->end());
    if (not radicals
                .emplace(*symbol, std::make_unique<RadicalData>(*symbol, name, std::move(matchSet)))
                .second) {
        Log(this).warn("Atom with duplicate symbol: '{0}' skipped.", *symbol);
        return false;
    }

    definition.logUnusedWarnings();
    return true;
}

bool AtomRepository::contains(const Symbol& symbol) const
{
    return atoms.contains(symbol) || radicals.contains(symbol);
}

const AtomData& AtomRepository::at(const Symbol& symbol) const
{
    if (const auto aIt = atoms.find(symbol); aIt != atoms.end())
        return *aIt->second;

    if (const auto rIt = radicals.find(symbol); rIt != radicals.end())
        return *rIt->second;

    Log(this).fatal("Tried to access an atom using an undefined symbol: '{0}'", symbol);
    CHG_UNREACHABLE();
}

size_t AtomRepository::totalDefinitionCount() const { return atoms.size() + radicals.size(); }

AtomRepository::AtomIterator AtomRepository::atomsBegin() const { return atoms.begin(); }

AtomRepository::AtomIterator AtomRepository::atomsEnd() const { return atoms.end(); }

AtomRepository::RadicalIterator AtomRepository::radicalsBegin() const { return radicals.begin(); }

AtomRepository::RadicalIterator AtomRepository::radicalsEnd() const { return radicals.end(); }

void AtomRepository::clear()
{
    atoms.clear();
    radicals.clear();
}
