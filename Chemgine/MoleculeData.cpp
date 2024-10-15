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

void MoleculeData::printDefinition(
	std::ostream& out, std::unordered_set<EstimatorId>& alreadyPrinted
) const
{
	meltingPointEstimator->printDefinition(out, alreadyPrinted, false);
	boilingPointEstimator->printDefinition(out, alreadyPrinted, false);
	solidDensityEstimator->printDefinition(out, alreadyPrinted, false);
	liquidDensityEstimator->printDefinition(out, alreadyPrinted, false);
	solidHeatCapacityEstimator->printDefinition(out, alreadyPrinted, false);
	liquidHeatCapacityEstimator->printDefinition(out, alreadyPrinted, false);
	fusionLatentHeatEstimator->printDefinition(out, alreadyPrinted, false);
	vaporizationLatentHeatEstimator->printDefinition(out, alreadyPrinted, false);
	sublimationLatentHeatEstimator->printDefinition(out, alreadyPrinted, false);
	relativeSolubilityEstimator->printDefinition(out, alreadyPrinted, false);
	henrysConstantEstimator->printDefinition(out, alreadyPrinted, false);

	out << '_' << Keywords::Types::Molecule;
	out << ':' << structure.toSMILES();
	out << '{';
	out << Keywords::Molecules::Name << ':' << name << ',';
	out << Keywords::Molecules::MeltingPoint << ':';
	meltingPointEstimator->printDefinition(out, alreadyPrinted, true);
	out << ',';
	out << Keywords::Molecules::BoilingPoint << ':';
	boilingPointEstimator->printDefinition(out, alreadyPrinted, true);
	out << ',';
	out << Keywords::Molecules::SolidDensity << ':';
	solidDensityEstimator->printDefinition(out, alreadyPrinted, true);
	out << ',';
	out << Keywords::Molecules::LiquidDensity << ':';
	liquidDensityEstimator->printDefinition(out, alreadyPrinted, true);
	out << ',';
	out << Keywords::Molecules::SolidHeatCapacity << ':';
	solidHeatCapacityEstimator->printDefinition(out, alreadyPrinted, true);
	out << ',';
	out << Keywords::Molecules::LiquidHeatCapacity << ':';
	liquidHeatCapacityEstimator->printDefinition(out, alreadyPrinted, true);
	out << ',';
	out << Keywords::Molecules::FusionLatentHeat << ':';
	fusionLatentHeatEstimator->printDefinition(out, alreadyPrinted, true);
	out << ',';
	out << Keywords::Molecules::VaporizationLatentHeat << ':';
	vaporizationLatentHeatEstimator->printDefinition(out, alreadyPrinted, true);
	out << ',';
	out << Keywords::Molecules::SublimationLatentHeat << ':';
	sublimationLatentHeatEstimator->printDefinition(out, alreadyPrinted, true);
	out << ',';
	out << Keywords::Molecules::RelativeSolubility << ':';
	relativeSolubilityEstimator->printDefinition(out, alreadyPrinted, true);
	out << ',';
	out << Keywords::Molecules::HenryConstant << ':';
	henrysConstantEstimator->printDefinition(out, alreadyPrinted, true);
	out << ',';
	out << Keywords::Molecules::Hydrophilicity << ':' << Def::print(polarity.hydrophilicity) << ',';
	out << Keywords::Molecules::Lipophilicity << ':' << Def::print(polarity.lipophilicity) << ',';
	out << Keywords::Molecules::Color << ':' << Def::print(color);
	out << "};\n";
}
