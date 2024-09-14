#include "MoleculeData.hpp"
#include "Log.hpp"

MoleculeData::MoleculeData(
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
) noexcept :
	id(id),
	structure(std::move(structure)),
	type(this->structure.isOrganic() ? MoleculeType::ORGANIC : MoleculeType::INORGANIC),
	name(name),
	polarity(hydrophilicity, lipophilicity),
	color(color),
	meltingPointEstimator(meltingPointEstimator),
	boilingPointEstimator(boilingPointEstimator),
	solidDensityEstimator(solidDensityEstimator),
	liquidDensityEstimator(liquidDensityEstimator),
	solidHeatCapacityEstimator(solidHeatCapacityEstimator),
	liquidHeatCapacityEstimator(liquidHeatCapacityEstimator),
	fusionLatentHeatEstimator(fusionLatentHeatEstimator),
	vaporizationLatentHeatEstimator(vaporizationLatentHeatEstimator),
	sublimationLatentHeatEstimator(sublimationLatentHeatEstimator),
	relativeSolubilityEstimator(relativeSolubilityEstimator),
	henrysConstantEstimator(henrysConstantEstimator)
{
	if (this->structure.isGeneric())
	{
		Log(this).warn("Generic structure with id {0} defined as molecule.", id);
	}
}

MoleculeData::MoleculeData(
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
) noexcept :
	id(id),
	structure(std::move(structure)),
	type(this->structure.isOrganic() ? MoleculeType::ORGANIC : MoleculeType::INORGANIC),
	name(this->structure.toSMILES()),
	polarity(hydrophilicity, lipophilicity),
	color(color),
	meltingPointEstimator(meltingPointEstimator),
	boilingPointEstimator(boilingPointEstimator),
	solidDensityEstimator(solidDensityEstimator),
	liquidDensityEstimator(liquidDensityEstimator),
	solidHeatCapacityEstimator(solidHeatCapacityEstimator),
	liquidHeatCapacityEstimator(liquidHeatCapacityEstimator),
	fusionLatentHeatEstimator(fusionLatentHeatEstimator),
	vaporizationLatentHeatEstimator(vaporizationLatentHeatEstimator),
	sublimationLatentHeatEstimator(sublimationLatentHeatEstimator),
	relativeSolubilityEstimator(relativeSolubilityEstimator),
	henrysConstantEstimator(henrysConstantEstimator)
{
	if (this->structure.isGeneric())
	{
		Log(this).warn("Generic structure with id {0} defined as molecule.", id);
	}
}

const MolecularStructure& MoleculeData::getStructure() const
{
	return structure;
}

std::string MoleculeData::getHRTag() const
{
	return '<' + std::to_string(id) + ':' + name + '>';
}
