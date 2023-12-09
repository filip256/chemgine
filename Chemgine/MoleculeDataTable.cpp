#include "MoleculeDataTable.hpp"
#include "DataHelpers.hpp"
#include "Logger.hpp"

#include <fstream>

MoleculeDataTable::MoleculeDataTable(const ApproximatorDataTable& approximators) noexcept :
	approximators(approximators)
{}


bool MoleculeDataTable::loadFromFile(const std::string& path)
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
			std::move(MoleculeData(id.result, line[2], line[1], approximators.at(103)))
		) == false)
		{
			Logger::log("Molecule with duplicate id " + std::to_string(id.result) + " skipped.", LogType::WARN);
		}
	}
	file.close();

	Logger::log("Loaded " + std::to_string(table.size()) + " molecules.", LogType::GOOD);

	return true;
}

size_t MoleculeDataTable::findFirst(const MolecularStructure& structure) const
{
	for (size_t i = 0; i < table.size(); ++i)
		if (table[i].getStructure() == structure)
			return i;

	return npos;
}

MoleculeIdType MoleculeDataTable::findOrAdd(MolecularStructure&& structure)
{
	const auto idx = findFirst(structure);
	if (idx != npos)
		return table[idx].id;

	const auto id = getFreeId();
	table.emplace(id, std::to_string(id), MoleculeData(id, std::move(structure), approximators.at(103)));
	return id;
}