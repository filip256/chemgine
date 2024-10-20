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
	static const uint8_t valueOffset = Utils::max(
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
		Def::Molecules::Color.size());

	DataDumper dump(out, valueOffset, 0, prettify);

	meltingPointEstimator->dumpDefinition(out, prettify, alreadyPrinted, false, 0);
	boilingPointEstimator->dumpDefinition(out, prettify, alreadyPrinted, false, 0);
	solidDensityEstimator->dumpDefinition(out, prettify, alreadyPrinted, false, 0);
	liquidDensityEstimator->dumpDefinition(out, prettify, alreadyPrinted, false, 0);
	solidHeatCapacityEstimator->dumpDefinition(out, prettify, alreadyPrinted, false, 0);
	liquidHeatCapacityEstimator->dumpDefinition(out, prettify, alreadyPrinted, false, 0);
	fusionLatentHeatEstimator->dumpDefinition(out, prettify, alreadyPrinted, false, 0);
	vaporizationLatentHeatEstimator->dumpDefinition(out, prettify, alreadyPrinted, false, 0);
	sublimationLatentHeatEstimator->dumpDefinition(out, prettify, alreadyPrinted, false, 0);
	relativeSolubilityEstimator->dumpDefinition(out, prettify, alreadyPrinted, false, 0);
	henrysConstantEstimator->dumpDefinition(out, prettify, alreadyPrinted, false, 0);

	dump.header(Def::Types::Molecule, structure, "")
		.beginProperties()
		.propertyWithSep(Def::Molecules::Name, name);

	if (prettify)
	{
		const auto subDefIndent = valueOffset + Def::Syntax::Indent.size() + 3;
		out << Def::Syntax::Indent << Def::Molecules::MeltingPoint << ':' << std::string(valueOffset - Def::Molecules::MeltingPoint.size(), ' ');
		meltingPointEstimator->dumpDefinition(out, true, alreadyPrinted, true, subDefIndent);
		out << ",\n";
		out << Def::Syntax::Indent << Def::Molecules::BoilingPoint << ':' << std::string(valueOffset - Def::Molecules::BoilingPoint.size(), ' ');
		boilingPointEstimator->dumpDefinition(out, true, alreadyPrinted, true, subDefIndent);
		out << ",\n";
		out << Def::Syntax::Indent << Def::Molecules::SolidDensity << ':' << std::string(valueOffset - Def::Molecules::SolidDensity.size(), ' ');
		solidDensityEstimator->dumpDefinition(out, true, alreadyPrinted, true, subDefIndent);
		out << ",\n";
		out << Def::Syntax::Indent << Def::Molecules::LiquidDensity << ':' << std::string(valueOffset - Def::Molecules::LiquidDensity.size(), ' ');
		liquidDensityEstimator->dumpDefinition(out, true, alreadyPrinted, true, subDefIndent);
		out << ",\n";
		out << Def::Syntax::Indent << Def::Molecules::SolidHeatCapacity << ':' << std::string(valueOffset - Def::Molecules::SolidHeatCapacity.size(), ' ');
		solidHeatCapacityEstimator->dumpDefinition(out, true, alreadyPrinted, true, subDefIndent);
		out << ",\n";
		out << Def::Syntax::Indent << Def::Molecules::LiquidHeatCapacity << ':' << std::string(valueOffset - Def::Molecules::LiquidHeatCapacity.size(), ' ');
		liquidHeatCapacityEstimator->dumpDefinition(out, true, alreadyPrinted, true, subDefIndent);
		out << ",\n";
		out << Def::Syntax::Indent << Def::Molecules::FusionLatentHeat << ':' << std::string(valueOffset - Def::Molecules::FusionLatentHeat.size(), ' ');
		fusionLatentHeatEstimator->dumpDefinition(out, true, alreadyPrinted, true, subDefIndent);
		out << ",\n";
		out << Def::Syntax::Indent << Def::Molecules::VaporizationLatentHeat << ':' << std::string(valueOffset - Def::Molecules::VaporizationLatentHeat.size(), ' ');
		vaporizationLatentHeatEstimator->dumpDefinition(out, true, alreadyPrinted, true, subDefIndent);
		out << ",\n";
		out << Def::Syntax::Indent << Def::Molecules::SublimationLatentHeat << ':' << std::string(valueOffset - Def::Molecules::SublimationLatentHeat.size(), ' ');
		sublimationLatentHeatEstimator->dumpDefinition(out, true, alreadyPrinted, true, subDefIndent);
		out << ",\n";
		out << Def::Syntax::Indent << Def::Molecules::RelativeSolubility << ':' << std::string(valueOffset - Def::Molecules::RelativeSolubility.size(), ' ');
		relativeSolubilityEstimator->dumpDefinition(out, true, alreadyPrinted, true, subDefIndent);
		out << ",\n";
		out << Def::Syntax::Indent << Def::Molecules::HenryConstant << ':' << std::string(valueOffset - Def::Molecules::HenryConstant.size(), ' ');
		henrysConstantEstimator->dumpDefinition(out, true, alreadyPrinted, true, subDefIndent);
		out << ",\n";
	}
	else
	{
		out << ':' << Def::print(structure);
		out << '{';
		out << Def::Molecules::Name << ':' << name << ',';
		out << Def::Molecules::MeltingPoint << ':';
		meltingPointEstimator->dumpDefinition(out, prettify, alreadyPrinted, true, 0);
		out << ',';
		out << Def::Molecules::BoilingPoint << ':';
		boilingPointEstimator->dumpDefinition(out, prettify, alreadyPrinted, true, 0);
		out << ',';
		out << Def::Molecules::SolidDensity << ':';
		solidDensityEstimator->dumpDefinition(out, prettify, alreadyPrinted, true, 0);
		out << ',';
		out << Def::Molecules::LiquidDensity << ':';
		liquidDensityEstimator->dumpDefinition(out, prettify, alreadyPrinted, true, 0);
		out << ',';
		out << Def::Molecules::SolidHeatCapacity << ':';
		solidHeatCapacityEstimator->dumpDefinition(out, prettify, alreadyPrinted, true, 0);
		out << ',';
		out << Def::Molecules::LiquidHeatCapacity << ':';
		liquidHeatCapacityEstimator->dumpDefinition(out, prettify, alreadyPrinted, true, 0);
		out << ',';
		out << Def::Molecules::FusionLatentHeat << ':';
		fusionLatentHeatEstimator->dumpDefinition(out, prettify, alreadyPrinted, true, 0);
		out << ',';
		out << Def::Molecules::VaporizationLatentHeat << ':';
		vaporizationLatentHeatEstimator->dumpDefinition(out, prettify, alreadyPrinted, true, 0);
		out << ',';
		out << Def::Molecules::SublimationLatentHeat << ':';
		sublimationLatentHeatEstimator->dumpDefinition(out, prettify, alreadyPrinted, true, 0);
		out << ',';
		out << Def::Molecules::RelativeSolubility << ':';
		relativeSolubilityEstimator->dumpDefinition(out, prettify, alreadyPrinted, true, 0);
		out << ',';
		out << Def::Molecules::HenryConstant << ':';
		henrysConstantEstimator->dumpDefinition(out, prettify, alreadyPrinted, true, 0);
		out << ',';
	}

	dump.propertyWithSep(Def::Molecules::Hydrophilicity, polarity.hydrophilicity)
		.propertyWithSep(Def::Molecules::Lipophilicity, polarity.lipophilicity)
		.propertyWithSep(Def::Molecules::Color, color)
		.endProperties()
		.endDefinition();
}
