#pragma once

#include "MolecularStructure.hpp"
#include "BaseApproximator.hpp"
#include "Amount.hpp"

#include <cstdint>
#include <string>


typedef uint32_t MoleculeIdType;

class MoleculeData
{
private:
	MolecularStructure structure;

public:
	const MoleculeIdType id;
	const std::string name;
	const BaseApproximator& meltingPointApproximator;
	const BaseApproximator& boilingPointApproximator;
	const BaseApproximator& solidDensityApproximator;
	const BaseApproximator& liquidDensityApproximator;
	const BaseApproximator& solidHeatCapacityApproximator;
	const BaseApproximator& liquidHeatCapacityApproximator;
	const BaseApproximator& fusionLatentHeatApproximator;
	const BaseApproximator& vaporizationLatentHeatApproximator;
	const BaseApproximator& sublimationLatentHeatApproximator;

	MoleculeData(
		const MoleculeIdType id,
		const std::string& name,
		const std::string& smiles,
		const BaseApproximator& meltingPointApproximator,
		const BaseApproximator& boilingPointApproximator,
		const BaseApproximator& solidDensityApproximator,
		const BaseApproximator& liquidDensityApproximator,
		const BaseApproximator& solidHeatCapacityApproximator,
		const BaseApproximator& liquidHeatCapacityApproximator,
		const BaseApproximator& fusionLatentHeatApproximator,
		const BaseApproximator& vaporizationLatentHeatApproximator,
		const BaseApproximator& sublimationLatentHeatApproximator
	) noexcept;

	MoleculeData(
		const MoleculeIdType id,
		MolecularStructure&& structure,
		const BaseApproximator& meltingPointApproximator,
		const BaseApproximator& boilingPointApproximator,
		const BaseApproximator& solidDensityApproximator,
		const BaseApproximator& liquidDensityApproximator,
		const BaseApproximator& solidHeatCapacityApproximator,
		const BaseApproximator& liquidHeatCapacityApproximator,
		const BaseApproximator& fusionLatentHeatApproximator,
		const BaseApproximator& vaporizationLatentHeatApproximator,
		const BaseApproximator& sublimationLatentHeatApproximator
	) noexcept;

	MoleculeData(const MoleculeData&) = delete;
	MoleculeData(MoleculeData&&) = default;
	~MoleculeData() = default;

	const MolecularStructure& getStructure() const;
};