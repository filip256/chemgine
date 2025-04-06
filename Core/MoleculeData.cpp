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
	static const auto valueOffset = checked_cast<uint8_t>(utils::max(
		def::Molecules::Name.size(),
		def::Molecules::MeltingPoint.size(),
		def::Molecules::BoilingPoint.size(),
		def::Molecules::SolidDensity.size(),
		def::Molecules::LiquidDensity.size(),
		def::Molecules::SolidHeatCapacity.size(),
		def::Molecules::LiquidHeatCapacity.size(),
		def::Molecules::FusionLatentHeat.size(),
		def::Molecules::VaporizationLatentHeat.size(),
		def::Molecules::SublimationLatentHeat.size(),
		def::Molecules::RelativeSolubility.size(),
		def::Molecules::HenryConstant.size(),
		def::Molecules::Hydrophilicity.size(),
		def::Molecules::Lipophilicity.size(),
		def::Molecules::Color.size()));

	def::DataDumper(out, valueOffset, 0, prettify)
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
		.header(def::Types::Molecule, structure, "")
		.beginProperties()
		.propertyWithSep(def::Molecules::Name, name)
		.subDefinitionWithSep(def::Molecules::MeltingPoint, meltingPointEstimator, alreadyPrinted)
		.subDefinitionWithSep(def::Molecules::BoilingPoint, boilingPointEstimator, alreadyPrinted)
		.subDefinitionWithSep(def::Molecules::SolidDensity, solidDensityEstimator, alreadyPrinted)
		.subDefinitionWithSep(def::Molecules::LiquidDensity, liquidDensityEstimator, alreadyPrinted)
		.subDefinitionWithSep(def::Molecules::SolidHeatCapacity, solidHeatCapacityEstimator, alreadyPrinted)
		.subDefinitionWithSep(def::Molecules::LiquidHeatCapacity, liquidHeatCapacityEstimator, alreadyPrinted)
		.subDefinitionWithSep(def::Molecules::FusionLatentHeat, fusionLatentHeatEstimator, alreadyPrinted)
		.subDefinitionWithSep(def::Molecules::VaporizationLatentHeat, vaporizationLatentHeatEstimator, alreadyPrinted)
		.subDefinitionWithSep(def::Molecules::SublimationLatentHeat, sublimationLatentHeatEstimator, alreadyPrinted)
		.subDefinitionWithSep(def::Molecules::RelativeSolubility, relativeSolubilityEstimator, alreadyPrinted)
		.subDefinitionWithSep(def::Molecules::HenryConstant, henrysConstantEstimator, alreadyPrinted)
		.propertyWithSep(def::Molecules::Hydrophilicity, polarity.hydrophilicity)
		.propertyWithSep(def::Molecules::Lipophilicity, polarity.lipophilicity)
		.property(def::Molecules::Color, color)
		.endProperties()
		.endDefinition();
}

void MoleculeData::print(std::ostream& out) const
{
	std::unordered_set<EstimatorId> history;
	dumpDefinition(out, true, history);
}
