#pragma once

#include "MoleculeId.hpp"
#include "MolecularStructure.hpp"
#include "EstimatorBase.hpp"
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

	const UnitizedEstimator<Unit::CELSIUS, Unit::TORR>& meltingPointEstimator;
	const UnitizedEstimator<Unit::CELSIUS, Unit::TORR>& boilingPointEstimator;

	const UnitizedEstimator<Unit::GRAM_PER_MILLILITER, Unit::CELSIUS>& solidDensityEstimator;
	const UnitizedEstimator<Unit::GRAM_PER_MILLILITER, Unit::CELSIUS>& liquidDensityEstimator;

	const UnitizedEstimator<Unit::JOULE_PER_MOLE_CELSIUS, Unit::TORR>& solidHeatCapacityEstimator;
	const UnitizedEstimator<Unit::JOULE_PER_MOLE_CELSIUS, Unit::TORR>& liquidHeatCapacityEstimator;

	const UnitizedEstimator<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR>& fusionLatentHeatEstimator;
	const UnitizedEstimator<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR>& vaporizationLatentHeatEstimator;
	const UnitizedEstimator<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR>& sublimationLatentHeatEstimator;

	const UnitizedEstimator<Unit::NONE, Unit::CELSIUS>& relativeSolubilityEstimator;
	const UnitizedEstimator<Unit::TORR_MOLE_RATIO, Unit::CELSIUS>& henrysConstantEstimator;

	MoleculeData(
		const MoleculeId id,
		const std::string& name,
		MolecularStructure&& structure,
		const Amount<Unit::MOLE_RATIO> hydrophilicity,
		const Amount<Unit::MOLE_RATIO> lipophilicity,
		const Color color,
		const UnitizedEstimator<Unit::CELSIUS, Unit::TORR>& meltingPointEstimator,
		const UnitizedEstimator<Unit::CELSIUS, Unit::TORR>& boilingPointEstimator,
		const UnitizedEstimator<Unit::GRAM_PER_MILLILITER, Unit::CELSIUS>& solidDensityEstimator,
		const UnitizedEstimator<Unit::GRAM_PER_MILLILITER, Unit::CELSIUS>& liquidDensityEstimator,
		const UnitizedEstimator<Unit::JOULE_PER_MOLE_CELSIUS, Unit::TORR>& solidHeatCapacityEstimator,
		const UnitizedEstimator<Unit::JOULE_PER_MOLE_CELSIUS, Unit::TORR>& liquidHeatCapacityEstimator,
		const UnitizedEstimator<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR>& fusionLatentHeatEstimator,
		const UnitizedEstimator<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR>& vaporizationLatentHeatEstimator,
		const UnitizedEstimator<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR>& sublimationLatentHeatEstimator,
		const UnitizedEstimator<Unit::NONE, Unit::CELSIUS>& relativeSolubilityEstimator,
		const UnitizedEstimator<Unit::TORR_MOLE_RATIO, Unit::CELSIUS>& henrysConstantEstimator
	) noexcept;

	MoleculeData(
		const MoleculeId id,
		MolecularStructure&& structure,
		const Amount<Unit::MOLE_RATIO> hydrophilicity,
		const Amount<Unit::MOLE_RATIO> lipophilicity,
		const Color color,
		const UnitizedEstimator<Unit::CELSIUS, Unit::TORR>& meltingPointEstimator,
		const UnitizedEstimator<Unit::CELSIUS, Unit::TORR>& boilingPointEstimator,
		const UnitizedEstimator<Unit::GRAM_PER_MILLILITER, Unit::CELSIUS>& solidDensityEstimator,
		const UnitizedEstimator<Unit::GRAM_PER_MILLILITER, Unit::CELSIUS>& liquidDensityEstimator,
		const UnitizedEstimator<Unit::JOULE_PER_MOLE_CELSIUS, Unit::TORR>& solidHeatCapacityEstimator,
		const UnitizedEstimator<Unit::JOULE_PER_MOLE_CELSIUS, Unit::TORR>& liquidHeatCapacityEstimator,
		const UnitizedEstimator<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR>& fusionLatentHeatEstimator,
		const UnitizedEstimator<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR>& vaporizationLatentHeatEstimator,
		const UnitizedEstimator<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR>& sublimationLatentHeatEstimator,
		const UnitizedEstimator<Unit::NONE, Unit::CELSIUS>& relativeSolubilityEstimator,
		const UnitizedEstimator<Unit::TORR_MOLE_RATIO, Unit::CELSIUS>& henrysConstantEstimator
	) noexcept;

	MoleculeData(const MoleculeData&) = delete;
	MoleculeData(MoleculeData&&) = default;
	~MoleculeData() = default;

	const MolecularStructure& getStructure() const;

	std::string getHRTag() const;
};
