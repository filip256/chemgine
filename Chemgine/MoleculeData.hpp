#pragma once

#include "MoleculeIdType.hpp"
#include "MolecularStructure.hpp"
#include "BaseEstimator.hpp"
#include "Amount.hpp"

#include <string>

class MoleculeData
{
private:
	MolecularStructure structure;

public:
	const MoleculeIdType id;
	const std::string name;
	const BaseEstimator& meltingPointEstimator;
	const BaseEstimator& boilingPointEstimator;
	const BaseEstimator& solidDensityEstimator;
	const BaseEstimator& liquidDensityEstimator;
	const BaseEstimator& solidHeatCapacityEstimator;
	const BaseEstimator& liquidHeatCapacityEstimator;
	const BaseEstimator& fusionLatentHeatEstimator;
	const BaseEstimator& vaporizationLatentHeatEstimator;
	const BaseEstimator& sublimationLatentHeatEstimator;

	MoleculeData(
		const MoleculeIdType id,
		const std::string& name,
		const std::string& smiles,
		const BaseEstimator& meltingPointEstimator,
		const BaseEstimator& boilingPointEstimator,
		const BaseEstimator& solidDensityEstimator,
		const BaseEstimator& liquidDensityEstimator,
		const BaseEstimator& solidHeatCapacityEstimator,
		const BaseEstimator& liquidHeatCapacityEstimator,
		const BaseEstimator& fusionLatentHeatEstimator,
		const BaseEstimator& vaporizationLatentHeatEstimator,
		const BaseEstimator& sublimationLatentHeatEstimator
	) noexcept;

	MoleculeData(
		const MoleculeIdType id,
		MolecularStructure&& structure,
		const BaseEstimator& meltingPointEstimator,
		const BaseEstimator& boilingPointEstimator,
		const BaseEstimator& solidDensityEstimator,
		const BaseEstimator& liquidDensityEstimator,
		const BaseEstimator& solidHeatCapacityEstimator,
		const BaseEstimator& liquidHeatCapacityEstimator,
		const BaseEstimator& fusionLatentHeatEstimator,
		const BaseEstimator& vaporizationLatentHeatEstimator,
		const BaseEstimator& sublimationLatentHeatEstimator
	) noexcept;

	MoleculeData(const MoleculeData&) = delete;
	MoleculeData(MoleculeData&&) = default;
	~MoleculeData() = default;

	const MolecularStructure& getStructure() const;
};