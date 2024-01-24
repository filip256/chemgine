#pragma once

#include "MoleculeIdType.hpp"
#include "MolecularStructure.hpp"

class GenericMoleculeData
{
private:
	MolecularStructure structure;

public:
	const MoleculeIdType id;

	GenericMoleculeData(
		const MoleculeIdType id,
		const std::string& smiles) noexcept;

	GenericMoleculeData(
		const MoleculeIdType id,
		MolecularStructure&& structure) noexcept;

	GenericMoleculeData(const GenericMoleculeData&) = delete;
	GenericMoleculeData(GenericMoleculeData&&) = default;
	~GenericMoleculeData() = default;

	const MolecularStructure& getStructure() const;
};