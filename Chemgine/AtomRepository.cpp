#include "AtomRepository.hpp"
#include "Parsers.hpp"
#include "Keywords.hpp"
#include "Log.hpp"
#include "Utils.hpp"

#include <fstream>

template <>
bool AtomRepository::add<AtomData>(DefinitionObject&& definition)
{
	const auto symbol = Def::parse<Symbol>(definition.getSpecifier());
	if (not symbol)
	{
		Log(this).error("Invalid atom symbol: '{0}' at: {1}.", definition.getSpecifier(), definition.getLocationName());
		return false;
	}

	const auto name = definition.pullProperty(Keywords::Atoms::Name);
	const auto weight = definition.pullProperty(Keywords::Atoms::Weight, Def::parseUnsigned<double>);
	auto valences = definition.pullProperty(Keywords::Atoms::Valences, Def::parse<std::vector<uint8_t>>);

	if (not(name && weight && valences))
	{
		Log(this).error("Incomplete atom definition at: {0}.", definition.getLocationName());
		return false;
	}

	const auto& ignored = definition.getRemainingProperties();
	for (const auto& [name, _] : ignored)
		Log(this).warn("Ignored unknown atom property: '{0}', at: {1}.", name, definition.getLocationName());
	
	if (add<AtomData>(*symbol, *name, *weight, std::move(*valences)) == false)
	{
		Log(this).warn("Atom with duplicate symbol: '{0}' skipped.", symbol->getAsString());
		return false;
	}

	return true;
}

template <>
bool AtomRepository::add<RadicalData>(DefinitionObject&& definition)
{
	const auto symbol = Def::parse<Symbol>(definition.getSpecifier());
	if (not symbol)
	{
		Log(this).error("Invalid radical symbol: '{0}' at: {1}.", definition.getSpecifier(), definition.getLocationName());
		return false;
	}

	const auto name = definition.pullDefaultProperty(Keywords::Atoms::Name, symbol->getAsString());
	const auto matches = definition.pullProperty(Keywords::Atoms::RadicalMatches, Def::parse<std::vector<Symbol>>);

	if (not matches)
	{
		Log(this).error("Incomplete radical definition at: {0}.", definition.getLocationName());
		return false;
	}

	// check match symbols
	for (size_t i = 0; i < matches->size(); ++i)
	{
		const auto& symbol = (*matches)[i];
		if (symbol == '*') // match-any symbol
		{
			if(matches->size() != 1)
				Log(this).warn("Found redundant atom match symbols in set containing the match-any symbol: '*', at: {0}.", definition.getLocationName());
			continue;
		}

		if (contains(symbol) == false)
		{
			Log(this).error("Unknown atom match symbol: '{0}', at: {1}.", symbol.getAsString(), definition.getLocationName());
			return false;
		}
	}

	const auto& ignored = definition.getRemainingProperties();
	for (const auto& [name, _] : ignored)
		Log(this).warn("Ignored unknown radical property: '{0}', at: {1}.", name, definition.getLocationName());

	std::unordered_set matchSet(matches->begin(), matches->end());
	if (add<RadicalData>(*symbol, name, std::move(matchSet)) == false)
	{
		Log(this).warn("Atom with duplicate symbol: '{0}' skipped.", symbol->getAsString());
		return false;
	}

	return true;
}

bool AtomRepository::contains(const Symbol symbol) const
{
	return table.contains(symbol);
}

const AtomData& AtomRepository::at(const Symbol symbol) const
{
	return *table.at(symbol);
}

AtomRepository::Iterator AtomRepository::begin() const
{
	return table.begin();
}

AtomRepository::Iterator AtomRepository::end() const
{
	return table.end();
}

void AtomRepository::clear()
{
	table.clear();
}
