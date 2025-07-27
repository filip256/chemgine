#include "RadicalData.hpp"
#include "Object.hpp"
#include "DataDumper.hpp"
#include "Keywords.hpp"
#include "Log.hpp"

const std::unordered_set<Symbol> RadicalData::MatchAny = std::unordered_set<Symbol>{ {'*'} };

RadicalData::RadicalData(
	const Symbol& symbol,
	const std::string& name,
	std::unordered_set<Symbol>&& matchables
) noexcept :
	AtomData(symbol, name, 0.0_g, utils::copy(AtomData::RadicalAnyValence)),
	matchables(std::move(matchables))
{}

void RadicalData::dumpDefinition(std::ostream& out, const bool prettify) const
{
	static const auto valueOffset = checked_cast<uint8_t>(utils::max(
		def::Atoms::Name.size(),
		def::Atoms::RadicalMatches.size()));

	def::DataDumper(out, valueOffset, 0, prettify)
		.header(def::Types::Radical, symbol, "")
		.beginProperties()
		.propertyWithSep(def::Atoms::Name, name)
		.property(def::Atoms::RadicalMatches, matchables)
		.endProperties()
		.endDefinition();
}
