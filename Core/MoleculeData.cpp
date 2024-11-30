#include "MoleculeData.hpp"
#include "DataDumper.hpp"
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
	GenericMoleculeData(id, std::move(structure)),
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

void MoleculeData::dumpDefinition(
	std::ostream& out,
	const bool prettify, 
	std::unordered_set<EstimatorId>& alreadyPrinted
) const
{
	static const auto valueOffset = checked_cast<uint8_t>(Utils::max(
		Def::Molecules::Name.size(),
		Def::Molecules::MeltingPoint.size(),
		Def::Molecules::BoilingPoint.size(),
		Def::Molecules::SolidDensity.size(),
		Def::Molecules::LiquidDensity.size(),
		Def::Molecules::SolidHeatCapacity.size(),
		Def::Molecules::LiquidHeatCapacity.size(),
		Def::Molecules::FusionLatentHeat.size(),
		Def::Molecules::VaporizationLatentHeat.size(),
		Def::Molecules::SublimationLatentHeat.size(),
		Def::Molecules::RelativeSolubility.size(),
		Def::Molecules::HenryConstant.size(),
		Def::Molecules::Hydrophilicity.size(),
		Def::Molecules::Lipophilicity.size(),
		Def::Molecules::Color.size()));

	Def::DataDumper(out, valueOffset, 0, prettify)
		.tryOolSubDefinition(meltingPointEstimator, alreadyPrinted)
		.tryOolSubDefinition(boilingPointEstimator, alreadyPrinted)
		.tryOolSubDefinition(solidDensityEstimator, alreadyPrinted)
		.tryOolSubDefinition(liquidDensityEstimator, alreadyPrinted)
		.tryOolSubDefinition(solidHeatCapacityEstimator, alreadyPrinted)
		.tryOolSubDefinition(liquidHeatCapacityEstimator, alreadyPrinted)
		.tryOolSubDefinition(fusionLatentHeatEstimator, alreadyPrinted)
		.tryOolSubDefinition(vaporizationLatentHeatEstimator, alreadyPrinted)
		.tryOolSubDefinition(sublimationLatentHeatEstimator, alreadyPrinted)
		.tryOolSubDefinition(relativeSolubilityEstimator, alreadyPrinted)
		.tryOolSubDefinition(henrysConstantEstimator, alreadyPrinted)
		.header(Def::Types::Molecule, structure, "")
		.beginProperties()
		.propertyWithSep(Def::Molecules::Name, name)
		.subDefinitionWithSep(Def::Molecules::MeltingPoint, meltingPointEstimator, alreadyPrinted)
		.subDefinitionWithSep(Def::Molecules::BoilingPoint, boilingPointEstimator, alreadyPrinted)
		.subDefinitionWithSep(Def::Molecules::SolidDensity, solidDensityEstimator, alreadyPrinted)
		.subDefinitionWithSep(Def::Molecules::LiquidDensity, liquidDensityEstimator, alreadyPrinted)
		.subDefinitionWithSep(Def::Molecules::SolidHeatCapacity, solidHeatCapacityEstimator, alreadyPrinted)
		.subDefinitionWithSep(Def::Molecules::LiquidHeatCapacity, liquidHeatCapacityEstimator, alreadyPrinted)
		.subDefinitionWithSep(Def::Molecules::FusionLatentHeat, fusionLatentHeatEstimator, alreadyPrinted)
		.subDefinitionWithSep(Def::Molecules::VaporizationLatentHeat, vaporizationLatentHeatEstimator, alreadyPrinted)
		.subDefinitionWithSep(Def::Molecules::SublimationLatentHeat, sublimationLatentHeatEstimator, alreadyPrinted)
		.subDefinitionWithSep(Def::Molecules::RelativeSolubility, relativeSolubilityEstimator, alreadyPrinted)
		.subDefinitionWithSep(Def::Molecules::HenryConstant, henrysConstantEstimator, alreadyPrinted)
		.propertyWithSep(Def::Molecules::Hydrophilicity, polarity.hydrophilicity)
		.propertyWithSep(Def::Molecules::Lipophilicity, polarity.lipophilicity)
		.property(Def::Molecules::Color, color)
		.endProperties()
		.endDefinition();
}

void MoleculeData::print(std::ostream& out) const
{
	std::unordered_set<EstimatorId> history;
	dumpDefinition(out, true, history);
}
