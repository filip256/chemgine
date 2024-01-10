#pragma once

#include "DataTable.hpp"
#include "MoleculeData.hpp"
#include "ApproximatorDataTable.hpp"

class MoleculeDataTable :
	public DataTable<MoleculeIdType, std::string, MoleculeData>
{
private:
	ApproximatorDataTable& approximators;

public:
	MoleculeDataTable(ApproximatorDataTable& approximators) noexcept;
	MoleculeDataTable(const MoleculeDataTable&) = delete;

	bool loadFromFile(const std::string& path);
	bool saveToFile(const std::string& path);

	size_t findFirst(const MolecularStructure& structure) const;

	MoleculeIdType findOrAdd(MolecularStructure&& structure);
};