#pragma once

#include "MoleculeId.hpp"
#include "MolecularStructure.hpp"
#include "BaseEstimator.hpp"
#include "Amount.hpp"
#include "MoleculeType.hpp"
#include "Polarity.hpp"
#include "Color.hpp"

#include <string>

class MoleculeData
{
private:
	MolecularStructure structure;

public:
	const MoleculeId id;
	const MoleculeType type;
	const std::string name;

	const Polarity polarity;

	const Color color;

	const BaseEstimator& meltingPointEstimator;
	const BaseEstimator& boilingPointEstimator;

	const BaseEstimator& solidDensityEstimator;
	const BaseEstimator& liquidDensityEstimator;

	const BaseEstimator& solidHeatCapacityEstimator;
	const BaseEstimator& liquidHeatCapacityEstimator;

	const BaseEstimator& fusionLatentHeatEstimator;
	const BaseEstimator& vaporizationLatentHeatEstimator;
	const BaseEstimator& sublimationLatentHeatEstimator;

	const BaseEstimator& relativeSolubilityEstimator;
	const BaseEstimator& henrysConstantEstimator;


	MoleculeData(
		const MoleculeId id,
		const std::string& name,
		const std::string& smiles,
		const Amount<Unit::MOLE_RATIO> hydrophilicity,
		const Amount<Unit::MOLE_RATIO> lipophilicity,
		const Color color,
		const BaseEstimator& meltingPointEstimator,
		const BaseEstimator& boilingPointEstimator,
		const BaseEstimator& solidDensityEstimator,
		const BaseEstimator& liquidDensityEstimator,
		const BaseEstimator& solidHeatCapacityEstimator,
		const BaseEstimator& liquidHeatCapacityEstimator,
		const BaseEstimator& fusionLatentHeatEstimator,
		const BaseEstimator& vaporizationLatentHeatEstimator,
		const BaseEstimator& sublimationLatentHeatEstimator,
		const BaseEstimator& relativeSolubilityEstimator,
		const BaseEstimator& henrysConstantEstimator
	) noexcept;

	MoleculeData(
		const MoleculeId id,
		MolecularStructure&& structure,
		const Amount<Unit::MOLE_RATIO> hydrophilicity,
		const Amount<Unit::MOLE_RATIO> lipophilicity,
		const Color color,
		const BaseEstimator& meltingPointEstimator,
		const BaseEstimator& boilingPointEstimator,
		const BaseEstimator& solidDensityEstimator,
		const BaseEstimator& liquidDensityEstimator,
		const BaseEstimator& solidHeatCapacityEstimator,
		const BaseEstimator& liquidHeatCapacityEstimator,
		const BaseEstimator& fusionLatentHeatEstimator,
		const BaseEstimator& vaporizationLatentHeatEstimator,
		const BaseEstimator& sublimationLatentHeatEstimator,
		const BaseEstimator& relativeSolubilityEstimator,
		const BaseEstimator& henrysConstantEstimator
	) noexcept;

	MoleculeData(const MoleculeData&) = delete;
	MoleculeData(MoleculeData&&) = default;
	~MoleculeData() = default;

	const MolecularStructure& getStructure() const;

	std::string getHRTag() const;
};
