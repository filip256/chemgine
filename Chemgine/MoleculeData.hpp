#pragma once

#include "MolecularStructure.hpp"
#include "BaseApproximator.hpp"

#include <cstdint>
#include <string>


typedef uint32_t MoleculeIdType;

class MoleculeData
{
private:
	const BaseApproximator& densityApproximator;
	MolecularStructure structure;

public:
	const MoleculeIdType id;
	const std::string name;

	MoleculeData(
		const MoleculeIdType id,
		const std::string& name,
		const std::string& smiles,
		const BaseApproximator& densityApproximator
	) noexcept;

	MoleculeData(
		const MoleculeIdType id,
		MolecularStructure&& structure,
		const BaseApproximator& densityApproximator
	) noexcept;

	MoleculeData(const MoleculeData&) = delete;
	MoleculeData(MoleculeData&&) = default;
	~MoleculeData() = default;

	const MolecularStructure& getStructure() const;
};