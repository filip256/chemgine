#include "GenericMoleculeRepository.hpp"
#include "Parsers.hpp"
#include "Log.hpp"
#include <fstream>

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
