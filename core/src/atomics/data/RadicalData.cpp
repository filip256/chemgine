#include "atomics/data/RadicalData.hpp"

#include "data/def/DataDumper.hpp"
#include "data/def/Keywords.hpp"
#include "data/def/Object.hpp"
#include "io/Log.hpp"

const std::unordered_set<Symbol> RadicalData::MatchAny = std::unordered_set<Symbol>{{'*'}};

RadicalData::RadicalData(const Symbol& symbol, const std::string& name, std::unordered_set<Symbol>&& matches) noexcept :
    AtomData(symbol, name, 0.0_g, utils::copy(AtomData::RadicalAnyValence)),
    matches(std::move(matches))
{}

const std::unordered_set<Symbol>& RadicalData::getMatches() const { return matches; }

void RadicalData::dumpDefinition(std::ostream& out, const bool prettify) const
{
    static constexpr auto valueOffset =
        checked_cast<uint8_t>(utils::max(def::Atoms::Name.size(), def::Atoms::RadicalMatches.size()));

    def::DataDumper(out, valueOffset, 0, prettify)
        .header(def::Types::Radical, symbol, "")
        .beginProperties()
        .property(def::Atoms::Name, name)
        .property(def::Atoms::RadicalMatches, matches)
        .endProperties()
        .endDefinition();
}
