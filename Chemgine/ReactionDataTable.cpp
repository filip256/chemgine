#include "ReactionDataTable.hpp"
#include "DataHelpers.hpp"
#include "Logger.hpp"

#include <fstream>

bool ReactionDataTable::loadFromFile(const std::string& path)
{
	std::ifstream file(path);

	if (!file.is_open())
	{
		return false;
		Logger::log("Failed to open file '" + path + "'.", LogType::BAD);
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
			Logger::log("Failed to load molecule due to empty SMILES string.", LogType::BAD);
			continue;
		}
		if (line[2].empty())
		{
			Logger::log("Molecule name was empty. (" + line[1] + ')', LogType::WARN);
		}

		const auto id = DataHelpers::toUInt(line[0]);

		if (id.status == 0)
		{
			Logger::log("Missing id, molecule '" + line[1] + "' skipped.", LogType::BAD);
			continue;
		}

		if (table.emplace(
			id.result,
			line[1],
			std::move(ReactionData(id.result, line[2], line[1]))
		) == false)
		{
			Logger::log("Molecule with duplicate id " + std::to_string(id.result) + " skipped.", LogType::WARN);
		}
	}
	file.close();

	Logger::log("Loaded " + std::to_string(table.size()) + " molecules.", LogType::GOOD);

	return true;
}