#include "MoleculeData.hpp"
#include "Log.hpp"

MoleculeData::MoleculeData(
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
) noexcept :
	id(id),
	structure(smiles),
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
