#pragma once

#include "DataTable.hpp"
#include "OrganicMoleculeData.hpp"

class MoleculeDataTable :
	public DataTable<ComponentIdType, std::string, OrganicMoleculeData>
{
public:
	MoleculeDataTable();
	MoleculeDataTable(const MoleculeDataTable&) = delete;

	bool loadFromFile(const std::string& path);
};