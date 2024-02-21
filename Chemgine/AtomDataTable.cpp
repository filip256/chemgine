#include "AtomDataTable.hpp"
#include "DataHelpers.hpp"
#include "Logger.hpp"
#include <fstream>

void AtomDataTable::addPredefined()
{
	table.emplace(101, "*", AtomData(101, "*", "Any Radical", 0, { 1 }));
	table.emplace(102, "R", AtomData(102, "R", "Alkyl Radical", 0, { 1 }));
	table.emplace(103, "A", AtomData(103, "A", "Aromatic Radical", 0, { 1 }));
	table.emplace(104, "X", AtomData(104, "X", "Halogen Radical", 0, { 1 }));
	table.emplace(105, "Me", AtomData(105, "Me", "Metal Radical", 0, { 1 }));

	table.emplace(199, "H", AtomData(199, "H", "Hydrogen", 1.008, { 1 }));
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

		const auto id = DataHelpers::parse<unsigned int>(line[0]);
		const auto weight = DataHelpers::parseUnsigned<double>(line[3]);
		auto valences = DataHelpers::parseList<uint8_t>(line[4], ';', true);

		if (valences.has_value() == false)
		{
			Logger::log("Atom with invalid or missing valence with id " + std::to_string(id.value()) + " skipped.", LogType::BAD);
			continue;
		}

		if (id.has_value() == false)
		{
			Logger::log("Missing id, atom '" + line[1] + "' skipped.", LogType::BAD);
			continue;
		}

		if (table.emplace(
			id.value(),
			line[1],
			AtomData(id.value(), line[1], line[2], Amount<Unit::GRAM>(weight.value_or(0)), std::move(*valences))
			) == false)
		{
			Logger::log("Duplicate atom with id " + std::to_string(id.value()) + " or symbol '" + line[1] + "' skipped.", LogType::WARN);
		}
	}
	file.close();

	Logger::log("Loaded " + std::to_string(table.size()) + " atoms.", LogType::INFO);

	return true;
}