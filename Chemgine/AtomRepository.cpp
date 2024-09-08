#include "AtomRepository.hpp"
#include "Parsers.hpp"
#include "Keywords.hpp"
#include "Log.hpp"
#include "Utils.hpp"

#include <fstream>

AtomRepository::~AtomRepository() noexcept
{
	for (size_t i = 0; i < table.size(); ++i)
		delete table[i];
}

bool AtomRepository::add(const AtomData* data)
{
	return table.emplace(data->id, data->symbol, std::move(data));
}

template <>
bool AtomRepository::add<AtomData>(DefinitionObject&& definition)
{
	const auto symbol = Def::parse<Symbol>(definition.getSpecifier());
	if (symbol.has_value() == false)
	{
		Log(this).error("Invalid atom symbol: '{0}' at: {1}.", definition.getSpecifier(), definition.getLocationName());
		return false;
	}

	const auto id = definition.pullProperty("id", Def::parseId<AtomId>);
	const auto name = definition.pullProperty(Keywords::Atoms::Name);
	const auto weight = definition.pullProperty(Keywords::Atoms::Weight, Def::parseUnsigned<double>);
	auto valences = definition.pullProperty(Keywords::Atoms::Valences, Def::parse<std::vector<uint8_t>>);

	if (id.has_value() == false || name.has_value() == false ||
		weight.has_value() == false || valences.has_value() == false)
	{
		Log(this).error("Incomplete atom definition at: {0}.", definition.getLocationName());
		return false;
	}

	const auto& ignored = definition.getRemainingProperties();
	for (const auto& [name, _] : ignored)
		Log(this).warn("Ignored unknown atom property: '{0}', at: {1}.", name, definition.getLocationName());
	
	if (add<AtomData>(*id, *symbol, *name, *weight, std::move(*valences)) == false)
	{
		Log(this).warn("Atom with duplicate id: '{0}' or symbol: '{1}' skipped.", *id, symbol->getAsString());
		return false;
	}

	return true;
}

template <>
bool AtomRepository::add<RadicalData>(DefinitionObject&& definition)
{
	const auto symbol = Def::parse<Symbol>(definition.getSpecifier());
	if (symbol.has_value() == false)
	{
		Log(this).error("Invalid radical symbol: '{0}' at: {1}.", definition.getSpecifier(), definition.getLocationName());
		return false;
	}

	const auto id = definition.pullProperty("id", Def::parseId<AtomId>);
	const auto name = definition.pullDefaultProperty(Keywords::Atoms::Name, symbol->getAsString());
	const auto matches = definition.pullProperty(Keywords::Atoms::RadicalMatches, Def::parse<std::vector<Symbol>>);

	if (id.has_value() == false || matches.has_value() == false)
	{
		Log(this).error("Incomplete radical definition at: {0}.", definition.getLocationName());
		return false;
	}

	std::unordered_set<AtomId> matchIds;
	matchIds.reserve(matches->size());
	for (size_t i = 0; i < matches->size(); ++i)
	{
		const auto& symbol = (*matches)[i];
		if (symbol == '*') // match-any symbol
		{
			matchIds = RadicalData::MatchAny;
			if(matches->size() != 1)
				Log(this).warn("Found redundant atom match symbols in set containing the match-any symbol: '*', at: {0}.", definition.getLocationName());
			break;
		}

		if (contains(symbol) == false)
		{
			Log(this).error("Unknown atom match symbol: '{0}', at: {1}.", symbol.getAsString(), definition.getLocationName());
			return false;
		}

		const auto id = at(symbol).id;
		if (matchIds.contains(id))
			Log(this).warn("Skipped duplicate atom match symbol: '{0}', at: {1}.",symbol.getAsString(), definition.getLocationName());

		matchIds.emplace(id);
	}

	const auto& ignored = definition.getRemainingProperties();
	for (const auto& [name, _] : ignored)
		Log(this).warn("Ignored unknown radical property: '{0}', at: {1}.", name, definition.getLocationName());

	if (add<RadicalData>(*id, *symbol, name, std::move(matchIds)) == false)
	{
		Log(this).warn("Atom with duplicate id: '{0}' or symbol: '{1}' skipped.", *id, symbol->getAsString());
		return false;
	}

	return true;
}

bool AtomRepository::contains(const AtomId id) const
{
	return table.containsKey1(id);
}

bool AtomRepository::contains(const Symbol symbol) const
{
	return table.containsKey2(symbol);
}

const AtomData& AtomRepository::at(const AtomId id) const
{
	return *table.atKey1(id);
}

const AtomData& AtomRepository::at(const Symbol symbol) const
{
	return *table.atKey2(symbol);
}
