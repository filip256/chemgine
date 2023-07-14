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
};