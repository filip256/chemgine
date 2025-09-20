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
    auto symbol = def::parse<Symbol>(definition.getSpecifier());
    if (not symbol) {
        Log(this).error("Invalid atom symbol: '{}' at: {}.", definition.getSpecifier(), definition.getLocationName());
        return false;
    }

    auto       name     = definition.getProperty(def::Atoms::Name);
    const auto weight   = definition.getProperty(def::Atoms::Weight, def::parse<Amount<Unit::GRAM_PER_MOLE>>);
    auto       valences = definition.getProperty(def::Atoms::Valences, def::parse<std::vector<uint8_t>>);

    if (not(name && weight && valences)) {
        Log(this).error("Incomplete atom definition at: {}.", definition.getLocationName());
        return false;
    }

    auto data = std::make_unique<AtomData>(std::move(*symbol), std::move(*name), *weight, std::move(*valences));
    if (not atoms.emplace(data->symbol, std::move(data)).second) {
        Log(this).warn("Atom with duplicate symbol: '{}' skipped.", data->symbol);
        return false;
    }

    definition.logUnusedWarnings();
    return true;
}

template <>
bool AtomRepository::add<RadicalData>(const def::Object& definition)
{
    auto symbol = def::parse<Symbol>(definition.getSpecifier());
    if (not symbol) {
        Log(this).error(
            "Invalid radical symbol: '{}' at: {}.", definition.getSpecifier(), definition.getLocationName());
        return false;
    }

    auto name    = definition.getDefaultProperty(def::Atoms::Name, utils::copy(symbol->str()));
    auto matches = definition.getProperty(def::Atoms::RadicalMatches, def::parse<std::vector<Symbol>>);

    if (not matches) {
        Log(this).error("Incomplete radical definition at: {}.", definition.getLocationName());
        return false;
    }

    // Check matching symbols and compute weight.
    SymbolMatchSet matchSet;
    matchSet.reserve(matches->size());
    Amount<Unit::GRAM_PER_MOLE> minWeight = 0.0f;
    for (auto&& matchSymbol : *matches) {
        if (matchSymbol == '*')  // Match-any symbol.
        {
            if (matches->size() != 1)
                Log(this).warn(
                    "Found redundant match symbols in set containing the match-any symbol: "
                    "'*', at: {}.",
                    definition.getLocationName());

            matchSet.emplace(std::move(matchSymbol), /*inferred=*/false);
            continue;
        }

        if (const auto matchAtom = find(matchSymbol)) {
            // The weight of a radical is the minimum weight of all the atoms and structures it matches. This factor
            // aids the early return condition of molecular comparisons.
            minWeight = std::min(minWeight, matchAtom->weight);
        }
        else {
            Log(this).error("Unknown atom match symbol: '{}', at: {}.", matchSymbol, definition.getLocationName());
            return false;
        }

        matchSet.emplace(std::move(matchSymbol), /*inferred=*/false);
    }

    // Infer cross-radical matches:
    // R1 = {A, B}     >>  R1 := {A, B}
    // R2 = {A, B, C}  >>  R2 := {A, B, C, R1, R3}
    // R3 = {A, B, C}  >>  R3 := {A, B, C, R1, R2}
    for (auto& [s, a] : atoms) {
        if (auto r = final_cast<RadicalData>(*a)) {
            const auto inclusion =
                utils::compareInclusion(matchSet, r->getMatches(), [](const auto& m) { return not m.isInferred; });
            if (utils::isNPos(inclusion))
                continue;  // No common matches.

            if (inclusion == 1)
                matchSet.emplace(utils::copy(r->symbol), /*inferred=*/true);
            else if (inclusion == -1)
                r->addInferredMatch(utils::copy(*symbol));
            else {
                Log(this).warn(
                    "Redundant radical definition: '{}' perfectly matches existing radical: '{}: {}', at: {}.",
                    *symbol,
                    r->symbol,
                    def::prettyPrint(r->getMatches()),
                    definition.getLocationName());

                r->addInferredMatch(utils::copy(*symbol));
                matchSet.emplace(utils::copy(r->symbol), /*inferred=*/true);
            }
        }
    }

    auto data = std::make_unique<RadicalData>(std::move(*symbol), std::move(name), minWeight, std::move(matchSet));
    if (not atoms.emplace(data->symbol, std::move(data)).second) {
        Log(this).warn("Radical with duplicate symbol: '{}' skipped.", data->symbol);
        return false;
    }

    definition.logUnusedWarnings();
    return true;
}

const AtomBaseData* AtomRepository::find(const Symbol& symbol) const
{
    const auto aIt = atoms.find(symbol);
    return aIt != atoms.end() ? aIt->second.get() : nullptr;
}

bool AtomRepository::contains(const Symbol& symbol) const { return atoms.contains(symbol); }

const AtomBaseData& AtomRepository::at(const Symbol& symbol) const
{
    if (const auto aIt = atoms.find(symbol); aIt != atoms.end())
        return *aIt->second;

    Log(this).fatal("Tried to access an atom using an undefined symbol: '{}'", symbol);
    CHG_UNREACHABLE();
}

size_t AtomRepository::totalDefinitionCount() const { return atoms.size(); }

AtomRepository::Iterator AtomRepository::begin() const { return atoms.begin(); }

AtomRepository::Iterator AtomRepository::end() const { return atoms.end(); }

void AtomRepository::clear() { atoms.clear(); }
