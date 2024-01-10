#include "FunctionalGroupDataTable.hpp"
#include "DataHelpers.hpp"
#include "Logger.hpp"
#include <fstream>


bool FunctionalGroupDataTable::loadFromFile(const std::string& path)
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

		if (line[1].empty())
		{
			Logger::log("Failed to load functional group due to empty SMILES string.", LogType::BAD);
			continue;
		}
		if (line[2].empty())
		{
			Logger::log("Functional group name was empty. (" + line[1] + ')', LogType::WARN);
		}

		const auto id = DataHelpers::toUInt(line[0]);

		if (id.has_value() == false)
		{
			Logger::log("Missing id, functional group '" + line[1] + "' skipped.", LogType::BAD);
			continue;
		}

		if (table.emplace(
			id.value(),
			line[1],
			std::move(FunctionalGroupData(id.value(), line[2], line[1]))
		) == false)
		{
			Logger::log("Functional group with duplicate id " + std::to_string(id.value()) + " skipped.", LogType::WARN);
		}
	}
	file.close();

	Logger::log("Loaded " + std::to_string(table.size()) + " functional groups.", LogType::GOOD);

	return true;
}

bool FunctionalGroupDataTable::saveToFile(const std::string& path)
{
	std::ofstream file(path);

	if (!file.is_open())
	{
		Logger::log("Failed to open file '" + path + "'.", LogType::BAD);
		return false;
	}

	for (size_t i = 0; i < table.size(); ++i)
	{
		const auto& e = table[i];
		file << e.id << ',' << e.getStructure().serialize() << ',' << e.name << '\n';
	}

	file.close();
	return true;
}

size_t FunctionalGroupDataTable::findFirst(const MolecularStructure& structure) const
{
	for (size_t i = 0; i < table.size(); ++i)
		if (table[i].getStructure() == structure)
			return i;

	return npos;
}

ComponentIdType FunctionalGroupDataTable::findOrAdd(MolecularStructure&& structure)
{
	if (structure.isEmpty())
		return 0;

	const auto idx = findFirst(structure);
	if (idx != npos)
		return table[idx].id;

	const auto id = getFreeId();
	table.emplace(id, std::to_string(id), FunctionalGroupData(id, "?", std::move(structure)));
	return id;
}