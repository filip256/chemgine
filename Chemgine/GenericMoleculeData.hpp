#pragma once

#include "MoleculeId.hpp"
#include "MolecularStructure.hpp"

class GenericMoleculeData
{
private:
	MolecularStructure structure;

public:
	const MoleculeId id;

	GenericMoleculeData(
		const MoleculeId id,
		MolecularStructure&& structure) noexcept;

	GenericMoleculeData(const GenericMoleculeData&) = delete;
	GenericMoleculeData(GenericMoleculeData&&) = default;
	~GenericMoleculeData() = default;

	const MolecularStructure& getStructure() const;

	std::string getHRTag() const;
};
