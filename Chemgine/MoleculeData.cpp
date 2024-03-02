#include "MoleculeData.hpp"
#include "Logger.hpp"

MoleculeData::MoleculeData(
	const MoleculeIdType id,
	const std::string& name,
	const std::string& smiles,
	const Amount<Unit::MOLE_RATIO> hydrophilicity,
	const Amount<Unit::MOLE_RATIO> lipophilicity,
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
) noexcept :
	id(id),
	structure(smiles),
	type(this->structure.isOrganic() ? MoleculeType::ORGANIC : MoleculeType::INORGANIC),
	name(name),
	polarity(hydrophilicity, lipophilicity),
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
	if (this->structure.isComplete() == false)
	{
		Logger::log("Incomplete structure with id " + std::to_string(id) + " defined as molecule.", LogType::WARN);
	}
}

MoleculeData::MoleculeData(
	const MoleculeIdType id,
	MolecularStructure&& structure,
	const Amount<Unit::MOLE_RATIO> hydrophilicity,
	const Amount<Unit::MOLE_RATIO> lipophilicity,
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
) noexcept :
	id(id),
	structure(std::move(structure)),
	type(this->structure.isOrganic() ? MoleculeType::ORGANIC : MoleculeType::INORGANIC),
	name(this->structure.print()),
	polarity(hydrophilicity, lipophilicity),
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
	if (this->structure.isComplete() == false)
	{
		Logger::log("Incomplete structure with id " + std::to_string(id) + " defined as molecule.", LogType::WARN);
	}
}

const MolecularStructure& MoleculeData::getStructure() const
{
	return structure;
}