#include "AtomDataTable.hpp"
#include "DataHelpers.hpp"
#include "Logger.hpp"
#include <fstream>

void AtomDataTable::addPredefined()
{
	table.emplace(101, "*", std::move(AtomData(101, "*", "Any Radical", 0, 1)));
	table.emplace(102, "R", std::move(AtomData(102, "R", "Alkyl Radical", 0, 1)));
	table.emplace(103, "A", std::move(AtomData(103, "A", "Aromatic Radical", 0, 1)));
	table.emplace(104, "X", std::move(AtomData(104, "X", "Halogen Radical", 0, 1)));
	table.emplace(105, "Me", std::move(AtomData(105, "Me", "Metal Radical", 0, 1)));
}

bool AtomDataTable::loadFromFile(const std::string& path)
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

		if (line[1].empty())
		{
			Logger::log("Failed to load atom due to empty symbol string.", LogType::BAD);
			continue;
		}
		if (line[2].empty())
		{
			Logger::log("Atom name was empty. (" + line[1] + ')', LogType::WARN);
		}

		const auto id = DataHelpers::toUInt(line[0]);
		auto weight = DataHelpers::toUDouble(line[3]);
		auto valence = DataHelpers::toUInt(line[4]);

		if (line[3].empty())
			weight = 0;
		if (line[4].empty())
			valence = 0;

		if (id.has_value() == false)
		{
			Logger::log("Missing id, atom '" + line[1] + "' skipped.", LogType::BAD);
			continue;
		}

		if (table.emplace(
			id.value(),
			line[1],
			std::move(AtomData(id.value(), line[1], line[2], weight.value(), valence.value()))
			) == false)
		{
			Logger::log("Duplicate atom with id " + std::to_string(id.value()) + " or symbol '" + line[1] + "' skipped.", LogType::WARN);
		}
	}
	file.close();

	if (table.containsKey2("H") == false)
	{
		auto id = getFreeId();
		table.emplace(id, "H", std::move(AtomData(id, "H", "Hydrogen", 1.008, 1)));
		Logger::log("Missing required atom 'H' created automatically with id " + std::to_string(id) + '.', LogType::WARN);
	}

	Logger::log("Loaded " + std::to_string(table.size()) + " atoms.", LogType::GOOD);

	return true;
}