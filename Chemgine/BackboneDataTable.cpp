#include "BackboneDataTable.hpp"
#include "DataHelpers.hpp"
#include "Logger.hpp"
#include <fstream>


bool BackboneDataTable::loadFromFile(const std::string& path)
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
			Logger::log("Failed to load backbone due to empty SMILES string.", LogType::BAD);
			continue;
		}
		if (line[2].empty())
		{
			Logger::log("Backbone name was empty. (" + line[1] + ')', LogType::WARN);
		}

		const auto id = DataHelpers::toUInt(line[0]);

		if (id.status == 0)
		{
			Logger::log("Missing id, backbone '" + line[1] + "' skipped.", LogType::BAD);
			continue;
		}

		if (table.emplace(
			id.result,
			line[1],
			std::move(BackboneData(id.result, line[2], line[1]))
		) == false)
		{
			Logger::log("Backbone with duplicate id " + std::to_string(id.result) + " skipped.", LogType::WARN);
		}
	}
	file.close();

	Logger::log("Loaded " + std::to_string(table.size()) + " backbones.", LogType::GOOD);

	return true;
}

size_t BackboneDataTable::findFirst(const std::string& smiles) const
{
	for (size_t i = 0; i < table.size(); ++i)
		if (table[i].getStructure() == smiles)
			return i;

	return npos;
}