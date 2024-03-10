#pragma once

#include "DataTable.hpp"
#include "MoleculeData.hpp"
#include "EstimatorDataTable.hpp"

class MoleculeDataTable :
	public DataTable<MoleculeId, std::string, MoleculeData>
{
private:
	EstimatorDataTable& estimators;

public:
	MoleculeDataTable(EstimatorDataTable& estimators) noexcept;
	MoleculeDataTable(const MoleculeDataTable&) = delete;

	bool loadFromFile(const std::string& path);
	bool saveToFile(const std::string& path);

	size_t findFirst(const MolecularStructure& structure) const;

	MoleculeId findOrAdd(MolecularStructure&& structure);
};