#include "atomics/data/RadicalData.hpp"

#include "atomics/data/AtomData.hpp"
#include "data/def/DataDumper.hpp"
#include "data/def/Keywords.hpp"
#include "data/def/Object.hpp"
#include "io/Log.hpp"

const ImmutableSet<uint8_t> RadicalData::AnyValence = {AtomData::NullValence};
const SymbolMatchSet        RadicalData::MatchAny   = SymbolMatchSet{
             {'*', false}
};

RadicalData::RadicalData(Symbol&& symbol, std::string&& name, SymbolMatchSet&& matches) noexcept :
    AtomBaseData(std::move(symbol), std::move(name)),
    matches(std::move(matches))
{}

const SymbolMatchSet& RadicalData::getMatches() const { return matches; }

void RadicalData::addInferredMatch(Symbol&& match) { matches.emplace(std::move(match), /*inferred=*/true); }

bool RadicalData::isRadical() const { return true; }

const ImmutableSet<uint8_t>& RadicalData::getValences() const { return AnyValence; }

uint8_t RadicalData::getFittingValence(const uint8_t bonds) const { return bonds != 0 ? bonds : 1; }

bool RadicalData::hasValence(const uint8_t) const { return true; }

uint8_t RadicalData::getPrecedence() const
{
    // During structure canonicalization radicals have the lowest precedence.
    return 0;
}

Amount<Unit::GRAM> RadicalData::getWeight() const
{
    // TODO: It might help to use the minimum weight of all the matching atoms, allowing more early returns in structure
    // comparisons.
    return 0.0_g;
}

void RadicalData::dumpDefinition(std::ostream& out, const bool prettify) const
{
    static constexpr auto valueOffset =
        checked_cast<uint8_t>(utils::max(def::Atoms::Name.size(), def::Atoms::RadicalMatches.size()));

    // Not ideal, but keeping both declared an inferred matches in the same container speeds-up simulation-time
    // querying.
    std::vector<Symbol> nonInferredMatches;
    nonInferredMatches.reserve(matches.size());
    for (const auto& match : matches)
        if (not match.isInferred)
            nonInferredMatches.emplace_back(match.getSymbol());

    def::DataDumper(out, valueOffset, 0, prettify)
        .header(def::Types::Radical, symbol, "")
        .beginProperties()
        .property(def::Atoms::Name, name)
        .property(def::Atoms::RadicalMatches, nonInferredMatches)
        .endProperties()
        .endDefinition();
}
