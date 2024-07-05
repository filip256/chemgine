#include "GenericMoleculeRepository.hpp"
#include "DataHelpers.hpp"
#include "Log.hpp"
#include <fstream>


bool GenericMoleculeRepository::loadFromFile(const std::string& path)
{
	std::ifstream file(path);

	if (!file.is_open())
	{
		Log(this).error("Failed to open file '{0}'.", path);
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
			Log(this).error("Failed to load generic molecule due to empty SMILES string.");
			continue;
		}

		const auto id = DataHelpers::parseId<MoleculeId>(line[0]);
		if (id.has_value() == false)
		{
			Log(this).error("Missing id, generic molecule: '{0}' skipped.", line[1]);
			continue;
		}

		if (table.emplace(
			*id,
			line[1],
			GenericMoleculeData(*id, line[1])
		) == false)
		{
			Log(this).warn("Generic molecule with duplicate id {0} skipped.", *id);
		}
	}
	file.close();

	Log(this).info("Loaded {0} generic molecules.", table.size());

	return true;
}

bool GenericMoleculeRepository::saveToFile(const std::string& path)
{
	std::ofstream file(path);

	if (!file.is_open())
	{
		Log(this).error("Failed to open file '{0}'.", path);
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
		Log(this).error("Tried to create a generic molecule from an empty structure.");
		return 0;
	}

	const auto idx = findFirst(structure);
	if (idx != npos)
		return table[idx].id;

	if (structure.isConcrete())
		Log(this).warn("New generic molecule created from concrete structure.");

	const auto id = getFreeId();
	table.emplace(id, std::to_string(id), GenericMoleculeData(id, std::move(structure)));
	return id;
}
