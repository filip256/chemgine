#pragma once

#include "MoleculeId.hpp"
#include "MolecularStructure.hpp"
#include "UnitizedEstimator.hpp"
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

	const EstimatorRef<Unit::CELSIUS, Unit::TORR> meltingPointEstimator;
	const EstimatorRef<Unit::CELSIUS, Unit::TORR> boilingPointEstimator;

	const EstimatorRef<Unit::GRAM_PER_MILLILITER, Unit::CELSIUS> solidDensityEstimator;
	const EstimatorRef<Unit::GRAM_PER_MILLILITER, Unit::CELSIUS> liquidDensityEstimator;

	const EstimatorRef<Unit::JOULE_PER_MOLE_CELSIUS, Unit::TORR> solidHeatCapacityEstimator;
	const EstimatorRef<Unit::JOULE_PER_MOLE_CELSIUS, Unit::TORR> liquidHeatCapacityEstimator;

	const EstimatorRef<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR> fusionLatentHeatEstimator;
	const EstimatorRef<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR> vaporizationLatentHeatEstimator;
	const EstimatorRef<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR> sublimationLatentHeatEstimator;

	const EstimatorRef<Unit::NONE, Unit::CELSIUS> relativeSolubilityEstimator;
	const EstimatorRef<Unit::TORR_MOLE_RATIO, Unit::CELSIUS> henrysConstantEstimator;

	MoleculeData(
		const MoleculeId id,
		const std::string& name,
		MolecularStructure&& structure,
		const Amount<Unit::MOLE_RATIO> hydrophilicity,
		const Amount<Unit::MOLE_RATIO> lipophilicity,
		const Color color,
		EstimatorRef<Unit::CELSIUS, Unit::TORR>&& meltingPointEstimator,
		EstimatorRef<Unit::CELSIUS, Unit::TORR>&& boilingPointEstimator,
		EstimatorRef<Unit::GRAM_PER_MILLILITER, Unit::CELSIUS>&& solidDensityEstimator,
		EstimatorRef<Unit::GRAM_PER_MILLILITER, Unit::CELSIUS>&& liquidDensityEstimator,
		EstimatorRef<Unit::JOULE_PER_MOLE_CELSIUS, Unit::TORR>&& solidHeatCapacityEstimator,
		EstimatorRef<Unit::JOULE_PER_MOLE_CELSIUS, Unit::TORR>&& liquidHeatCapacityEstimator,
		EstimatorRef<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR>&& fusionLatentHeatEstimator,
		EstimatorRef<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR>&& vaporizationLatentHeatEstimator,
		EstimatorRef<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR>&& sublimationLatentHeatEstimator,
		EstimatorRef<Unit::NONE, Unit::CELSIUS>&& relativeSolubilityEstimator,
		EstimatorRef<Unit::TORR_MOLE_RATIO, Unit::CELSIUS>&& henrysConstantEstimator
	) noexcept;

	MoleculeData(const MoleculeData&) = delete;
	MoleculeData(MoleculeData&&) = default;
	~MoleculeData() = default;

	const MolecularStructure& getStructure() const;

	std::string getHRTag() const;

	void printDefinition(
		std::ostream& out, std::unordered_set<EstimatorId>& alreadyPrinted
	) const;
};
