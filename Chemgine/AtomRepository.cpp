#include "AtomRepository.hpp"
#include "DynamicAmount.hpp"
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

	const auto name = definition.getProperty(Def::Atoms::Name);
	const auto weight = definition.getProperty(Def::Atoms::Weight, Def::parse<Amount<Unit::GRAM>>);
	auto valences = definition.getProperty(Def::Atoms::Valences, Def::parse<std::vector<uint8_t>>);

	if (not(name && weight && valences))
	{
		Log(this).error("Incomplete atom definition at: {0}.", definition.getLocationName());
		return false;
	}
	
	if (not atomTable.emplace(*symbol,
		std::make_unique<AtomData>(*symbol, *name, *weight, std::move(*valences))).second)
	{
		Log(this).warn("Atom with duplicate symbol: '{0}' skipped.", symbol->getString());
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

	const auto name = definition.getDefaultProperty(Def::Atoms::Name, Utils::copy(symbol->getString()));
	const auto matches = definition.getProperty(Def::Atoms::RadicalMatches, Def::parse<std::vector<Symbol>>);

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
			Log(this).error("Unknown atom match symbol: '{0}', at: {1}.", symbol.getString(), definition.getLocationName());
			return false;
		}
	}

	std::unordered_set matchSet(matches->begin(), matches->end());
	if (not radicalTable.emplace(*symbol, 
		std::make_unique<RadicalData>(*symbol, name, std::move(matchSet))).second)
	{
		Log(this).warn("Atom with duplicate symbol: '{0}' skipped.", symbol->getString());
		return false;
	}

	return true;
}

bool AtomRepository::contains(const Symbol& symbol) const
{
	return atomTable.contains(symbol) || radicalTable.contains(symbol);
}

const AtomData& AtomRepository::at(const Symbol& symbol) const
{
	if (const auto aIt = atomTable.find(symbol); aIt != atomTable.end())
		return *aIt->second;

	if (const auto rIt = radicalTable.find(symbol); rIt != radicalTable.end())
		return *rIt->second;

	Log(this).fatal("Tried to access an atom suing an undefined symbol: '{0}'", symbol.getString());
}

AtomRepository::AtomIterator AtomRepository::atomsBegin() const
{
	return atomTable.begin();
}

AtomRepository::AtomIterator AtomRepository::atomsEnd() const
{
	return atomTable.end();
}

AtomRepository::RadicalIterator AtomRepository::radicalsBegin() const
{
	return radicalTable.begin();
}

AtomRepository::RadicalIterator AtomRepository::radicalsEnd() const
{
	return radicalTable.end();
}

void AtomRepository::clear()
{
	atomTable.clear();
	radicalTable.clear();
}
