#pragma once

#include "DataTable.hpp"
#include "OrganicMoleculeData.hpp"
#include "FunctionalGroupDataTable.hpp"

class MoleculeDataTable :
	public DataTable<ComponentIdType, std::string, OrganicMoleculeData>
{
public:
	MoleculeDataTable() = default;
	MoleculeDataTable(const MoleculeDataTable&) = delete;

	bool loadFromFile(const std::string& path);

	size_t findFirst(const std::string& smiles) const;
	size_t findFirst(const MolecularStructure& structure) const;

	MoleculeIdType findOrAdd(MolecularStructure&& structure);
};