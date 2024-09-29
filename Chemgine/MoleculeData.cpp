#include "MoleculeData.hpp"
#include "Log.hpp"

MoleculeData::MoleculeData(
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
) noexcept :
	id(id),
	structure(std::move(structure)),
	type(this->structure.isOrganic() ? MoleculeType::ORGANIC : MoleculeType::INORGANIC),
	name(name),
	polarity(hydrophilicity, lipophilicity),
	color(color),
	meltingPointEstimator(std::move(meltingPointEstimator)),
	boilingPointEstimator(std::move(boilingPointEstimator)),
	solidDensityEstimator(std::move(solidDensityEstimator)),
	liquidDensityEstimator(std::move(liquidDensityEstimator)),
	solidHeatCapacityEstimator(std::move(solidHeatCapacityEstimator)),
	liquidHeatCapacityEstimator(std::move(liquidHeatCapacityEstimator)),
	fusionLatentHeatEstimator(std::move(fusionLatentHeatEstimator)),
	vaporizationLatentHeatEstimator(std::move(vaporizationLatentHeatEstimator)),
	sublimationLatentHeatEstimator(std::move(sublimationLatentHeatEstimator)),
	relativeSolubilityEstimator(std::move(relativeSolubilityEstimator)),
	henrysConstantEstimator(std::move(henrysConstantEstimator))
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
