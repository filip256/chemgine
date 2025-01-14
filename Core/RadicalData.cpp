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
	AtomData(symbol, name, 0.0 * _Gram, Utils::copy(AtomData::RadicalAnyValence)),
	matchables(std::move(matchables))
{}

void RadicalData::dumpDefinition(std::ostream& out, const bool prettify) const
{
	static const auto valueOffset = checked_cast<uint8_t>(Utils::max(
		Def::Atoms::Name.size(),
		Def::Atoms::RadicalMatches.size()));

	Def::DataDumper(out, valueOffset, 0, prettify)
		.header(Def::Types::Radical, symbol, "")
		.beginProperties()
		.propertyWithSep(Def::Atoms::Name, name)
		.property(Def::Atoms::RadicalMatches, matchables)
		.endProperties()
		.endDefinition();
}
