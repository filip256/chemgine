#pragma once

#include "MolecularStructure.hpp"
#include "FunctionalGroupDataTable.hpp"

#include <cstdint>
#include <string>


typedef uint32_t MoleculeIdType;

class OrganicMoleculeData
{
private:
	MolecularStructure structure;

public:
	OrganicMoleculeData(
		const MoleculeIdType id,
		const std::string& name,
		const std::string& smiles) noexcept;

	OrganicMoleculeData(const OrganicMoleculeData&) = delete;
	OrganicMoleculeData(OrganicMoleculeData&&) = default;
	~OrganicMoleculeData() = default;

	void categorize(const FunctionalGroupDataTable& patterns);
};