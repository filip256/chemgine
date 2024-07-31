#include "AtomRepository.hpp"
#include "DataHelpers.hpp"
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
	const auto symbol = DataHelpers::parse<Symbol>(definition.specifier);
	if (symbol.has_value() == false)
	{
		Log(this).error("Invalid atom symbol: '{0}' at: {1}.", definition.specifier, definition.getLocationName());
		return false;
	}

	const auto id = definition.pullProperty("id", DataHelpers::parseId<AtomId>);
	const auto name = definition.pullProperty(Keywords::Atoms::Name);
	const auto weight = definition.pullProperty(Keywords::Atoms::Weight, DataHelpers::parseUnsigned<double>);
	auto valences = definition.pullProperty(Keywords::Atoms::Valences, DataHelpers::parseList<uint8_t>, ',', true);

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
	const auto symbol = DataHelpers::parse<Symbol>(definition.specifier);
	if (symbol.has_value() == false)
	{
		Log(this).error("Invalid radical symbol: '{0}' at: {1}.", definition.specifier, definition.getLocationName());
		return false;
	}

	const auto id = definition.pullProperty("id", DataHelpers::parseId<AtomId>);
	const auto name = definition.pullDefaultProperty(Keywords::Atoms::Name, symbol->getAsString());
	const auto matches = definition.pullProperty(Keywords::Atoms::RadicalMatches, DataHelpers::parseList<Symbol>, ',', true);

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

std::unordered_set<AtomId> AtomRepository::getIds(const std::vector<Symbol>& symbols) const
{
	std::unordered_set<AtomId> result;
	result.reserve(symbols.size());

	for (const auto& s : symbols)
	{
		if (contains(s) == false)
		{
			Log(this).error("Skipped undefined atom symbol: '{0}'.", s.getAsString());
			continue;
		}
		result.emplace(at(s).id);
	}

	return result;
}

bool AtomRepository::loadFromFile(const std::string& path)
{
	std::ifstream file(path);

	if (!file.is_open())
	{
		Log(this).error("Failed to open file '{0}'.", path);
		return false;
	}

	//if (files::verifyChecksum(file).code != 200) //not OK
	//	return StatusCode<>::FileCorrupted;

	table.clear();

	//parse file
	std::string buffer;
	std::getline(file, buffer);
	while (std::getline(file, buffer))
	{
		auto line = Utils::split(buffer, ',');

		const auto id = DataHelpers::parseId<AtomId>(line[0]);
		if (id.has_value() == false)
		{
			Log(this).error("Missing id, atom skipped.");
			continue;
		}

		const auto symbol = DataHelpers::parse<Symbol>(line[1]);
		if (symbol.has_value() == false)
		{
			Log(this).error("Missing or invalid symbol: \"{0}\", atom skipped.", line[1]);
			continue;
		}

		if (line[2].empty())
		{
			Log(this).warn("Missing name for atom '{0}'.", line[1]);
		}

		const auto weight = DataHelpers::parseUnsigned<double>(line[3]);

		auto valences = DataHelpers::parseList<uint8_t>(line[4], ';', true);
		if (valences.has_value() == false)
		{
			Log(this).error("Atom with invalid or missing valence with id {0} skipped.", *id);
			continue;
		}

		if (add<AtomData>(*id, *symbol, line[2], Amount<Unit::GRAM>(weight.value_or(0)), std::move(*valences)) == false)
		{
			Log(this).warn("Duplicate atom with id {0} or symbol '{1}' skipped.", *id, line[1]);
		}
	}
	file.close();

	Log(this).info("Loaded {0} atoms.", table.size());

	return true;
}
