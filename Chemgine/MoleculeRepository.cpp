#include "MoleculeRepository.hpp"
#include "Parsers.hpp"
#include "DefinitionParsers.hpp"
#include "OffsetEstimator.hpp"
#include "ScaleEstimator.hpp"
#include "SplineEstimator.hpp"
#include "TypedEstimator.hpp"
#include "Keywords.hpp"
#include "Log.hpp"

#include <fstream>

MoleculeRepository::MoleculeRepository(EstimatorRepository& estimators) noexcept :
	estimators(estimators)
{}

bool MoleculeRepository::add(DefinitionObject&& definition)
{
	auto structure = Def::parse<MolecularStructure>(definition.getSpecifier());
	if (structure.has_value() == false)
	{
		Log(this).error("Invalid SMILES specifier: '{0}', as: {1}.", definition.getSpecifier(), definition.getLocationName());
		return false;
	}

	const auto id = definition.pullProperty("id", Def::parseId<MoleculeId>);
	const auto name = definition.pullDefaultProperty(Keywords::Molecules::Name, "?");

	if (id.has_value() == false)
		return false;

	const auto mp = definition.pullDefaultProperty(Keywords::Molecules::MeltingPoint, 0.0,
		Def::parse<double>);
	const auto bp = definition.pullDefaultProperty(Keywords::Molecules::BoilingPoint, 100.0,
		Def::parse<double>);
	const auto sd = definition.pullDefaultProperty(Keywords::Molecules::SolidDensity, Spline<float>({ {0, 1.0} }),
		Def::parse<Spline<float>>);
	const auto ld = definition.pullDefaultProperty(Keywords::Molecules::LiquidDensity, Spline<float>({ {0, 1.0} }),
		Def::parse<Spline<float>>);
	const auto shc = definition.pullDefaultProperty(Keywords::Molecules::SolidHeatCapacity, Spline<float>({ {36.0, 760.0} }),
		Def::parse<Spline<float>>);
	const auto lhc = definition.pullDefaultProperty(Keywords::Molecules::LiquidHeatCapacity, Spline<float>({ {75.4840232, 760.0} }),
		Def::parse<Spline<float>>);
	const auto flh = definition.pullDefaultProperty(Keywords::Molecules::FusionLatentHeat, 6020.0,
		Def::parse<double>);
	const auto vlh = definition.pullDefaultProperty(Keywords::Molecules::VaporizationLatentHeat, 40700.0,
		Def::parse<double>);
	const auto slh = definition.pullDefaultProperty(Keywords::Molecules::SublimationLatentHeat, std::numeric_limits<double>::max(),
		Def::parse<double>);
	const auto hp = definition.pullDefaultProperty(Keywords::Molecules::Hydrophilicity, 1.0,
		Def::parse<double>);
	const auto lp = definition.pullDefaultProperty(Keywords::Molecules::Lipophilicity, 0.0,
		Def::parse<double>);
	const auto invSol = definition.pullDefaultProperty(Keywords::Molecules::InverseSolubility, false,
		Def::parse<bool>);
	const auto sol = definition.pullOptionalProperty(Keywords::Molecules::Solubility,
		Def::parse<Spline<float>>);
	const auto henry = definition.pullDefaultProperty(Keywords::Molecules::HenryConstant, Spline<float>({ {1000.0, 760.0} }),
		Def::parse<Spline<float>>);
	const auto col = definition.pullDefaultProperty(Keywords::Molecules::Color, Color(0, 255, 255, 100),
		Def::parse<Color>);

	const auto subDef = definition.getOptionalDefinition("subdef",
		Def::parse<TypedEstimator<Unit::LITER, Unit::GRAM>*>);
	const auto subDef1 = definition.getOptionalDefinition("subdefr",
		Def::parse<TypedEstimator<Unit::GRAM_PER_MILLILITER, Unit::CELSIUS>*>);

	const auto& mpA = estimators.add<OffsetEstimator>(estimators.at(toId(BuiltinEstimator::TORR_TO_REL_BP)), mp);
	const auto& bpA = estimators.add<OffsetEstimator>(estimators.at(toId(BuiltinEstimator::TORR_TO_REL_BP)), bp);
	const auto& sdA = estimators.add<SplineEstimator>(sd);
	const auto& ldA = estimators.add<SplineEstimator>(ld);
	const auto& shcA = estimators.add<SplineEstimator>(shc);
	const auto& lhcA = estimators.add<SplineEstimator>(lhc);
	const auto& flhA = estimators.add<ScaleEstimator>(estimators.at(toId(BuiltinEstimator::TDIF_TORR_TO_REL_LH)), flh);
	const auto& vlhA = estimators.add<ScaleEstimator>(estimators.at(toId(BuiltinEstimator::TDIF_TORR_TO_REL_LH)), vlh);
	const auto& slhA = estimators.add<ScaleEstimator>(estimators.at(toId(BuiltinEstimator::TDIF_TORR_TO_REL_LH)), slh);
	const auto& solA = sol.has_value() ? estimators.add<SplineEstimator>(*sol) :
		invSol ? estimators.at(toId(BuiltinEstimator::TEMP_TO_REL_INV_SOL)) :
		estimators.at(toId(BuiltinEstimator::TEMP_TO_REL_SOL));
	const auto& henryA = estimators.add<SplineEstimator>(henry);

	const auto& ignored = definition.getRemainingProperties();
	for (const auto& [name, _] : ignored)
		Log(this).warn("Ignored unknown molecule property: '{0}', at: {1}.", name, definition.getLocationName());

	if (table.emplace(
		*id, name,
		MoleculeData(*id, name, std::move(*structure), hp, lp, col, mpA, bpA, sdA, ldA, shcA, lhcA, flhA, vlhA, slhA, solA, henryA)) == false)
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

	const auto& mpA = estimators.add<OffsetEstimator>(estimators.at(toId(BuiltinEstimator::TORR_TO_REL_BP)), 0);
	const auto& bpA = estimators.add<OffsetEstimator>(estimators.at(toId(BuiltinEstimator::TORR_TO_REL_BP)), 100);
	const auto& sdA = estimators.add<SplineEstimator>(Spline<float>({ {0, 1.0f} }));
	const auto& ldA = estimators.add<SplineEstimator>(Spline<float>({ {0, 1.0f} }));
	const auto& shcA = estimators.add<SplineEstimator>(Spline<float>({ {36.0, 760.0} }));
	const auto& lhcA = estimators.add<SplineEstimator>(Spline<float>({ {75.4840232, 760.0} }));
	const auto& flhA = estimators.add<ScaleEstimator>(estimators.at(toId(BuiltinEstimator::TDIF_TORR_TO_REL_LH)), 6020.0f);
	const auto& vlhA = estimators.add<ScaleEstimator>(estimators.at(toId(BuiltinEstimator::TDIF_TORR_TO_REL_LH)), 40700.0f);
	const auto& slhA = estimators.add<ScaleEstimator>(estimators.at(toId(BuiltinEstimator::TDIF_TORR_TO_REL_LH)), std::numeric_limits<double>::max());
	const auto hydro = 1.0;
	const auto lipo = 0.0;
	const auto& solA = estimators.at(toId(BuiltinEstimator::TEMP_TO_REL_SOL));
	const auto& henryA = estimators.add<SplineEstimator>(Spline<float>({ {1000.0, 760.0} }));
	const auto color = Color(0, 255, 255, 100);

	const auto id = getFreeId();
	table.emplace(id, std::to_string(id), MoleculeData(id, std::move(structure), hydro, lipo, color, mpA, bpA, sdA, ldA, shcA, lhcA, flhA, vlhA, slhA, solA, henryA));
	return id;
}
