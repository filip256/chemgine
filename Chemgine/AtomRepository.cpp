#include "AtomRepository.hpp"
#include "DataHelpers.hpp"
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

void AtomRepository::loadBuiltins()
{
	if(contains("H") == false)
		add<AtomData>(101, "H", "Hydrogen", 1.008, std::vector<uint8_t> { 1 });

	add<RadicalData>(21, "*", "any radical", Utils::copy(RadicalData::MatchAny));
	add<RadicalData>(22, "R", "alkyl radical", std::unordered_set<Symbol> { "C" });
	add<RadicalData>(23, "A", "aromatic radical", std::unordered_set<Symbol> { "C" });
	add<RadicalData>(24, "X", "halogen radical", std::unordered_set<Symbol> { "F", "Cl", "Br", "At", "Ts" });
	add<RadicalData>(25, "Me", "metal radical", std::unordered_set<Symbol> { "Li", "Na", "K", "Be", "Mg", "Ca", "Sr", "Ba" });
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

std::unordered_set<AtomId> AtomRepository::getIds(const std::unordered_set<Symbol>& symbols) const
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
		auto line = DataHelpers::parseList(buffer, ',');

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

	loadBuiltins();

	return true;
}
