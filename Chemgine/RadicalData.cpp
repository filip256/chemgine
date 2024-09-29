#include "RadicalData.hpp"
#include "DefinitionObject.hpp"
#include "Keywords.hpp"
#include "Utils.hpp"
#include "Log.hpp"

const std::unordered_set<Symbol> RadicalData::MatchAny = std::unordered_set<Symbol>{ {'*'} };

RadicalData::RadicalData(
	const Symbol symbol,
	const std::string& name,
	std::unordered_set<Symbol>&& matchables
) noexcept :
	AtomData(symbol, name, 0.0_g, Utils::copy(AtomData::RadicalAnyValence)),
	matchables(std::move(matchables))
{}

DefinitionObject RadicalData::toDefinition() const
{
	std::unordered_map<std::string, std::string> properties
	{
		{Keywords::Atoms::Name, name},
		{Keywords::Atoms::RadicalMatches, Def::print(matchables)},
	};

	return DefinitionObject(
		DefinitionType::ATOM, "", symbol.getAsString(),
		std::move(properties), {}, {},
		DefinitionLocation::createUnknown());
}
