#include "MoleculeData.hpp"
#include "Logger.hpp"

MoleculeData::MoleculeData(
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
) noexcept :
	id(id),
	name(name),
	structure(smiles),
	meltingPointApproximator(meltingPointApproximator),
	boilingPointApproximator(boilingPointApproximator),
	solidDensityApproximator(solidDensityApproximator),
	liquidDensityApproximator(liquidDensityApproximator),
	solidHeatCapacityApproximator(solidHeatCapacityApproximator),
	liquidHeatCapacityApproximator(liquidHeatCapacityApproximator),
	fusionLatentHeatApproximator(fusionLatentHeatApproximator),
	vaporizationLatentHeatApproximator(vaporizationLatentHeatApproximator),
	sublimationLatentHeatApproximator(sublimationLatentHeatApproximator)
{
	if (this->structure.isComplete() == false)
	{
		Logger::log("Incomplete structure with id " + std::to_string(id) + " defined as molecule.", LogType::WARN);
	}
}

MoleculeData::MoleculeData(
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
) noexcept :
	id(id),
	name("?"),
	structure(std::move(structure)),
	meltingPointApproximator(meltingPointApproximator),
	boilingPointApproximator(boilingPointApproximator),
	solidDensityApproximator(solidDensityApproximator),
	liquidDensityApproximator(liquidDensityApproximator),
	solidHeatCapacityApproximator(solidHeatCapacityApproximator),
	liquidHeatCapacityApproximator(liquidHeatCapacityApproximator),
	fusionLatentHeatApproximator(fusionLatentHeatApproximator),
	vaporizationLatentHeatApproximator(vaporizationLatentHeatApproximator),
	sublimationLatentHeatApproximator(sublimationLatentHeatApproximator)
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