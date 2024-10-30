#pragma once

#include "MoleculeId.hpp"
#include "MolecularStructure.hpp"

class GenericMoleculeData
{
public:
	const MoleculeId id;

protected:
	const MolecularStructure structure;

public:
	GenericMoleculeData(
		const MoleculeId id,
		MolecularStructure&& structure
	) noexcept;

	GenericMoleculeData(const GenericMoleculeData&) = delete;
	GenericMoleculeData(GenericMoleculeData&&) = default;
	~GenericMoleculeData() = default;

	const MolecularStructure& getStructure() const;
};
