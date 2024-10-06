#include "RadicalData.hpp"
#include "DefinitionObject.hpp"
#include "Keywords.hpp"
#include "Utils.hpp"
#include "Log.hpp"

const std::unordered_set<Symbol> RadicalData::MatchAny = std::unordered_set<Symbol>{ {'*'} };

RadicalData::RadicalData(
	const Symbol& symbol,
	const std::string& name,
	std::unordered_set<Symbol>&& matchables
) noexcept :
	AtomData(symbol, name, 0.0_g, Utils::copy(AtomData::RadicalAnyValence)),
	matchables(std::move(matchables))
{}

void RadicalData::printDefinition(std::ostream& out) const
{
	out << '_' << Keywords::Types::Radical;
	out << ':' << Def::print(symbol);
	out << '{';
	out << Keywords::Atoms::Name << ':' << name << ',';
	out << Keywords::Atoms::RadicalMatches << ':' << Def::print(matchables);
	out << "};\n";
}
