#include "FunctionalGroupDataTable.hpp"
#include "DataHelpers.hpp"
#include "Logger.hpp"
#include <fstream>

FunctionalGroupDataTable::FunctionalGroupDataTable() : DataTable<ComponentIdType, std::string, FunctionalGroupData>()
{

}

bool FunctionalGroupDataTable::loadFromFile(const std::string& path)
{
	std::ifstream file(path);

	if (!file.is_open())
		return false;

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

		if (id.status == 0)
		{
			Logger::log("Missing id, functional group '" + line[1] + "' skipped.", LogType::BAD);
			continue;
		}
		if (table.containsKey1(id.result))
		{
			Logger::log("Functional group with duplicate id " + std::to_string(id.result) + " skipped.", LogType::WARN);
			continue;
		}

		if (table.emplace(
			id.result,
			line[1],
			std::move(FunctionalGroupData(id.result, line[2], std::move(MolecularStructure(line[1]))))
		) == false)
		{
			Logger::log("Insertion of functional group with id " + std::to_string(id.result) + " failed unexpectedly.", LogType::WARN);
		}
	}
	file.close();

	return true;
}