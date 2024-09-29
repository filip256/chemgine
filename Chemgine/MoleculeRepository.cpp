#include "MoleculeRepository.hpp"
#include "Parsers.hpp"
#include "EstimatorParsers.hpp"
#include "AffineEstimator.hpp"
#include "SplineEstimator.hpp"
#include "UnitizedEstimator.hpp"
#include "Keywords.hpp"
#include "Log.hpp"

#include <fstream>

MoleculeRepository::MoleculeRepository(EstimatorRepository& estimators) noexcept :
	estimators(estimators)
{}

bool MoleculeRepository::add(DefinitionObject&& definition)
{
	auto structure = Def::Parser<MolecularStructure>::parse(definition.getSpecifier());
	if (not structure)
	{
		Log(this).error("Invalid SMILES specifier: '{0}', as: {1}.", definition.getSpecifier(), definition.getLocationName());
		return false;
	}

	const auto id = definition.pullProperty("id", Def::parseId<MoleculeId>);
	if (not id)
		return false;

	const auto name = definition.pullDefaultProperty(Keywords::Molecules::Name, "?");
	const auto hp = definition.pullDefaultProperty(Keywords::Molecules::Hydrophilicity, 1.0,
		Def::parse<double>);
	const auto lp = definition.pullDefaultProperty(Keywords::Molecules::Lipophilicity, 0.0,
		Def::parse<double>);
	const auto col = definition.pullDefaultProperty(Keywords::Molecules::Color, Color(0, 255, 255, 100),
		Def::parse<Color>);
	auto mp = definition.getDefinition(Keywords::Molecules::MeltingPoint,
		Def::Parser<UnitizedEstimator<Unit::CELSIUS, Unit::TORR>>::parse, estimators);
	auto bp = definition.getDefinition(Keywords::Molecules::BoilingPoint,
		Def::Parser<UnitizedEstimator<Unit::CELSIUS, Unit::TORR>>::parse, estimators);
	auto sd = definition.getDefinition(Keywords::Molecules::SolidDensity,
		Def::Parser<UnitizedEstimator<Unit::GRAM_PER_MILLILITER, Unit::CELSIUS>>::parse, estimators);
	auto ld = definition.getDefinition(Keywords::Molecules::LiquidDensity,
		Def::Parser<UnitizedEstimator<Unit::GRAM_PER_MILLILITER, Unit::CELSIUS>>::parse, estimators);
	auto shc = definition.getDefinition(Keywords::Molecules::SolidHeatCapacity,
		Def::Parser<UnitizedEstimator<Unit::JOULE_PER_MOLE_CELSIUS, Unit::TORR>>::parse, estimators);
	auto lhc = definition.getDefinition(Keywords::Molecules::LiquidHeatCapacity,
		Def::Parser<UnitizedEstimator<Unit::JOULE_PER_MOLE_CELSIUS, Unit::TORR>>::parse, estimators);
	auto flh = definition.getDefinition(Keywords::Molecules::FusionLatentHeat,
		Def::Parser<UnitizedEstimator<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR>>::parse, estimators);
	auto vlh = definition.getDefinition(Keywords::Molecules::VaporizationLatentHeat,
		Def::Parser<UnitizedEstimator<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR>>::parse, estimators);
	auto slh = definition.getDefinition(Keywords::Molecules::SublimationLatentHeat,
		Def::Parser<UnitizedEstimator<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR>>::parse, estimators);
	auto sol = definition.getDefinition(Keywords::Molecules::RelativeSolubility,
		Def::Parser<UnitizedEstimator<Unit::NONE, Unit::CELSIUS>>::parse, estimators);
	auto hen = definition.getDefinition(Keywords::Molecules::HenryConstant,
		Def::Parser<UnitizedEstimator<Unit::TORR_MOLE_RATIO, Unit::CELSIUS>>::parse, estimators);

	const auto& ignored = definition.getRemainingProperties();
	for (const auto& [name, _] : ignored)
		Log(this).warn("Ignored unknown molecule property: '{0}', at: {1}.", name, definition.getLocationName());

	if (table.emplace(
		*id, name,
		MoleculeData(
			*id, name, std::move(*structure),
			hp, lp, col, 
			std::move(*mp), std::move(*bp),
			std::move(*sd), std::move(*ld),
			std::move(*shc), std::move(*lhc), std::move(*flh),
			std::move(*vlh), std::move(*slh), std::move(*sol), std::move(*hen))) == false)
	{
		Log(this).warn("Molecule with duplicate id: '{0}' skipped.", *id);
		return false;
	}

	return true;
}

bool MoleculeRepository::saveToFile(const std::string& path)
{
	std::ofstream file(path);

	if (!file.is_open())
	{
		Log(this).error("Failed to open file '{0}'.", path);
		return false;
	}

	for (size_t i = 0; i < table.size(); ++i)
	{
		const auto& e = table[i];
		file << '#' << e.id << ',' << e.getStructure().toSMILES() << ',' << e.name << ',' << ',' << ',' << ',' << ',' << ',' << ',' << ',' << ',' << ',' << ',' << ',' << ',' << ',' << '\n';
	}

	file.close();
	return true;
}

size_t MoleculeRepository::findFirst(const MolecularStructure& structure) const
{
	for (size_t i = 0; i < table.size(); ++i)
		if (table[i].getStructure() == structure)
			return i;

	return npos;
}

MoleculeId MoleculeRepository::findOrAdd(MolecularStructure&& structure)
{
	if (structure.isEmpty() || structure.isGeneric())
	{
		Log(this).error("Tried to create a concrete molecule from an empty or generic structure.");
		return 0;
	}

	const auto idx = findFirst(structure);
	if (idx != npos)
		return table[idx].id;

	Log(this).debug("New structure discovered: \n{0}", structure.print());

	const auto hydro = 1.0;
	const auto lipo = 0.0;
	const auto color = Color(0, 255, 255, 100);
	auto mp = estimators.add<ConstantEstimator<Unit::CELSIUS, Unit::TORR>>(0.0);
	auto bp = estimators.add<ConstantEstimator<Unit::CELSIUS, Unit::TORR>>(100.0);
	auto sd = estimators.add<ConstantEstimator<Unit::GRAM_PER_MILLILITER, Unit::CELSIUS>>(1.0);
	auto ld = estimators.add<ConstantEstimator<Unit::GRAM_PER_MILLILITER, Unit::CELSIUS>>(1.0);
	auto shc = estimators.add<ConstantEstimator<Unit::JOULE_PER_MOLE_CELSIUS, Unit::TORR>>(36.0);
	auto lhc = estimators.add<ConstantEstimator<Unit::JOULE_PER_MOLE_CELSIUS, Unit::TORR>>(75.4840232);
	auto flh = estimators.add<ConstantEstimator<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR>>(6020.0);
	auto vlh = estimators.add<ConstantEstimator<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR>>(40700.0);
	auto slh = estimators.add<ConstantEstimator<Unit::JOULE_PER_MOLE, Unit::CELSIUS, Unit::TORR>>(std::numeric_limits<float>::max());
	auto sol = estimators.add<ConstantEstimator<Unit::NONE, Unit::CELSIUS>>(1.0);
	auto hen = estimators.add<ConstantEstimator<Unit::TORR_MOLE_RATIO, Unit::CELSIUS>>(1000.0);

	const auto id = getFreeId();
	table.emplace(id, std::to_string(id), MoleculeData(
		id, structure.toSMILES(), std::move(structure),
		hydro, lipo, color,
		std::move(mp), std::move(bp),
		std::move(sd), std::move(ld),
		std::move(shc), std::move(lhc),
		std::move(flh), std::move(vlh), std::move(slh),
		std::move(sol), std::move(hen)));

	return id;
}
