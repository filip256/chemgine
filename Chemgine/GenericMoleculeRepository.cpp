#include "GenericMoleculeRepository.hpp"
#include "DataHelpers.hpp"
#include "Logger.hpp"
#include <fstream>


bool GenericMoleculeRepository::loadFromFile(const std::string& path)
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
			Logger::log("Failed to load generic molecule due to empty SMILES string.", LogType::BAD);
			continue;
		}

		const auto id = DataHelpers::parseId<MoleculeId>(line[0]);
		if (id.has_value() == false)
		{
			Logger::log("Missing id, generic molecule: '" + line[1] + "' skipped.", LogType::BAD);
			continue;
		}

		if (table.emplace(
			*id,
			line[1],
			GenericMoleculeData(*id, line[1])
		) == false)
		{
			Logger::log("Generic molecule with duplicate id " + std::to_string(*id) + " skipped.", LogType::WARN);
		}
	}
	file.close();

	Logger::log("Loaded " + std::to_string(table.size()) + " generic molecules.", LogType::INFO);

	return true;
}

bool GenericMoleculeRepository::saveToFile(const std::string& path)
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
		file << '#' << e.id << ',' << e.getStructure().toSMILES() << '\n';
	}

	file.close();
	return true;
}

size_t GenericMoleculeRepository::findFirst(const MolecularStructure& structure) const
{
	for (size_t i = 0; i < table.size(); ++i)
		if (table[i].getStructure() == structure)
			return i;

	return npos;
}

MoleculeId GenericMoleculeRepository::findOrAdd(MolecularStructure&& structure)
{
	if (structure.isEmpty())
	{
		Logger::log("Tried to create a generic molecule from an empty structure.", LogType::BAD);
		return 0;
	}

	const auto idx = findFirst(structure);
	if (idx != npos)
		return table[idx].id;

	if (structure.isConcrete())
		Logger::log("New generic molecule created from concrete structure.", LogType::WARN);

	const auto id = getFreeId();
	table.emplace(id, std::to_string(id), GenericMoleculeData(id, std::move(structure)));
	return id;
}
