#pragma once

#include "DataTable.hpp"
#include "GenericMoleculeData.hpp"

class GenericMoleculeDataTable :
	public DataTable<MoleculeId, std::string, GenericMoleculeData>
{
public:
	GenericMoleculeDataTable() = default;
	GenericMoleculeDataTable(const GenericMoleculeDataTable&) = delete;

	bool loadFromFile(const std::string& path);
	bool saveToFile(const std::string& path);

	size_t findFirst(const MolecularStructure& structure) const;

	MoleculeId findOrAdd(MolecularStructure&& structure);
};