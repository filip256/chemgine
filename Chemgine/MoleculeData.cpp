#include "MoleculeData.hpp"
#include "Logger.hpp"

MoleculeData::MoleculeData(
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
) noexcept :
	id(id),
	name(name),
	structure(smiles),
	meltingPointEstimator(meltingPointEstimator),
	boilingPointEstimator(boilingPointEstimator),
	solidDensityEstimator(solidDensityEstimator),
	liquidDensityEstimator(liquidDensityEstimator),
	solidHeatCapacityEstimator(solidHeatCapacityEstimator),
	liquidHeatCapacityEstimator(liquidHeatCapacityEstimator),
	fusionLatentHeatEstimator(fusionLatentHeatEstimator),
	vaporizationLatentHeatEstimator(vaporizationLatentHeatEstimator),
	sublimationLatentHeatEstimator(sublimationLatentHeatEstimator)
{
	if (this->structure.isComplete() == false)
	{
		Logger::log("Incomplete structure with id " + std::to_string(id) + " defined as molecule.", LogType::WARN);
	}
}

MoleculeData::MoleculeData(
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
) noexcept :
	id(id),
	name("?"),
	structure(std::move(structure)),
	meltingPointEstimator(meltingPointEstimator),
	boilingPointEstimator(boilingPointEstimator),
	solidDensityEstimator(solidDensityEstimator),
	liquidDensityEstimator(liquidDensityEstimator),
	solidHeatCapacityEstimator(solidHeatCapacityEstimator),
	liquidHeatCapacityEstimator(liquidHeatCapacityEstimator),
	fusionLatentHeatEstimator(fusionLatentHeatEstimator),
	vaporizationLatentHeatEstimator(vaporizationLatentHeatEstimator),
	sublimationLatentHeatEstimator(sublimationLatentHeatEstimator)
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