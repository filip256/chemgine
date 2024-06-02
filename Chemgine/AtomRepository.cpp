#include "AtomRepository.hpp"
#include "DataHelpers.hpp"
#include "Logger.hpp"
#include <fstream>

void AtomRepository::addPredefined()
{
	table.emplace(21, "*", AtomData(21, "*", "any radical", 0, { 1 }));
	table.emplace(22, "R", AtomData(22, "R", "alkyl radical", 0, { 1 }));
	table.emplace(23, "A", AtomData(23, "A", "aromatic radical", 0, { 1 }));
	table.emplace(24, "X", AtomData(24, "X", "halogen radical", 0, { 1 }));
	table.emplace(25, "Me", AtomData(25, "Me", "metal radical", 0, { 1 }));

	table.emplace(101, "H", AtomData(101, "H", "Hydrogen", 1.008, { 1 }));
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
	addPredefined();

	//parse file
	std::string buffer;
	std::getline(file, buffer);
	while (std::getline(file, buffer))
	{
		auto line = DataHelpers::parseList(buffer, ',');

		const auto id = DataHelpers::parseId<ComponentId>(line[0]);
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

		if (table.emplace(
			*id,
			line[1],
			AtomData(*id, *symbol, line[2], Amount<Unit::GRAM>(weight.value_or(0)), std::move(*valences))
			) == false)
		{
			Logger::log("Duplicate atom with id " + std::to_string(*id) + " or symbol '" + line[1] + "' skipped.", LogType::WARN);
		}
	}
	file.close();

	Logger::log("Loaded " + std::to_string(table.size()) + " atoms.", LogType::INFO);

	return true;
}
