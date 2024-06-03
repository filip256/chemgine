#include "AtomRepository.hpp"
#include "DataHelpers.hpp"
#include "Logger.hpp"
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
			Logger::log("AtomRepository: Skipped undefined atom symbol: " + s.getAsString() + ".", LogType::BAD);
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
		Logger::log("Failed to open file '" + path + "'.", LogType::BAD);
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
			Logger::log("Missing id, atom skipped.", LogType::BAD);
			continue;
		}

		const auto symbol = DataHelpers::parse<Symbol>(line[1]);
		if (symbol.has_value() == false)
		{
			Logger::log("Missing or invalid symbol: \"" + line[1] + "\", atom skipped.", LogType::BAD);
			continue;
		}

		if (line[2].empty())
		{
			Logger::log("Atom name was empty. (" + line[1] + ')', LogType::WARN);
		}

		const auto weight = DataHelpers::parseUnsigned<double>(line[3]);

		auto valences = DataHelpers::parseList<uint8_t>(line[4], ';', true);
		if (valences.has_value() == false)
		{
			Logger::log("Atom with invalid or missing valence with id " + std::to_string(*id) + " skipped.", LogType::BAD);
			continue;
		}

		if (add<AtomData>(*id, *symbol, line[2], Amount<Unit::GRAM>(weight.value_or(0)), std::move(*valences)) == false)
		{
			Logger::log("Duplicate atom with id " + std::to_string(*id) + " or symbol '" + line[1] + "' skipped.", LogType::WARN);
		}
	}
	file.close();

	Logger::log("Loaded " + std::to_string(table.size()) + " atoms.", LogType::INFO);

	loadBuiltins();

	return true;
}
