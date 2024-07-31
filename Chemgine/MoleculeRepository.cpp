#include "MoleculeRepository.hpp"
#include "DataHelpers.hpp"
#include "OffsetEstimator.hpp"
#include "ScaleEstimator.hpp"
#include "SplineEstimator.hpp"
#include "Keywords.hpp"
#include "Log.hpp"

#include <fstream>

MoleculeRepository::MoleculeRepository(EstimatorRepository& estimators) noexcept :
	estimators(estimators)
{}


bool MoleculeRepository::add(DefinitionObject&& definition)
{
	auto structure = DataHelpers::parse<MolecularStructure>(definition.specifier);
	if (structure.has_value() == false)
	{
		Log(this).error("Invalid SMILES specifier: '{0}', as: {1}.", definition.specifier, definition.getLocationName());
		return false;
	}

	const auto id = definition.pullProperty("id", DataHelpers::parseId<MoleculeId>);
	const auto name = definition.pullDefaultProperty(Keywords::Molecules::Name, "?");

	if (id.has_value() == false)
		return false;

	const auto mp = definition.pullDefaultProperty(Keywords::Molecules::MeltingPoint, 0.0,
		DataHelpers::parse<double>);
	const auto bp = definition.pullDefaultProperty(Keywords::Molecules::BoilingPoint, 100.0,
		DataHelpers::parse<double>);
	const auto sd = definition.pullDefaultProperty(Keywords::Molecules::SolidDensity, Spline<float>({ {0, 1.0} }),
		DataHelpers::parse<Spline<float>>);
	const auto ld = definition.pullDefaultProperty(Keywords::Molecules::LiquidDensity, Spline<float>({ {0, 1.0} }),
		DataHelpers::parse<Spline<float>>);
	const auto shc = definition.pullDefaultProperty(Keywords::Molecules::SolidHeatCapacity, Spline<float>({ {36.0, 760.0} }),
		DataHelpers::parse<Spline<float>>);
	const auto lhc = definition.pullDefaultProperty(Keywords::Molecules::LiquidHeatCapacity, Spline<float>({ {75.4840232, 760.0} }),
		DataHelpers::parse<Spline<float>>);
	const auto flh = definition.pullDefaultProperty(Keywords::Molecules::FusionLatentHeat, 6020.0,
		DataHelpers::parse<double>);
	const auto vlh = definition.pullDefaultProperty(Keywords::Molecules::VaporizationLatentHeat, 40700.0,
		DataHelpers::parse<double>);
	const auto slh = definition.pullDefaultProperty(Keywords::Molecules::SublimationLatentHeat, std::numeric_limits<double>::max(),
		DataHelpers::parse<double>);
	const auto hp = definition.pullDefaultProperty(Keywords::Molecules::Hydrophilicity, 1.0,
		DataHelpers::parse<double>);
	const auto lp = definition.pullDefaultProperty(Keywords::Molecules::Lipophilicity, 0.0,
		DataHelpers::parse<double>);
	const auto invSol = definition.pullDefaultProperty(Keywords::Molecules::InverseSolubility, false,
		DataHelpers::parse<bool>);
	const auto sol = definition.pullOptionalProperty(Keywords::Molecules::Solubility,
		DataHelpers::parse<Spline<float>>);
	const auto henry = definition.pullDefaultProperty(Keywords::Molecules::HenryConstant, Spline<float>({ {1000.0, 760.0} }),
		DataHelpers::parse<Spline<float>>);
	const auto col = definition.pullDefaultProperty(Keywords::Molecules::Color, Color(0, 255, 255, 100),
		DataHelpers::parse<Color>);

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

bool MoleculeRepository::loadFromFile(const std::string& path)
{
	std::ifstream file(path);

	if (!file.is_open())
	{
		Log(this).error("Failed to open file '{0}'.", path);
		return false;
	}

	//if (files::verifyChecksum(file).code != 200) //not OK
	//	return StatusCode<>::FileCorrupted;

	table.clear();

	//parse file
	std::string buffer;
	std::getline(file, buffer);
	while (std::getline(file, buffer))
	{
		auto line = Utils::split(buffer, ',');

		if (line.size() != 17)
		{
			Log(this).error("Incompletely defined molecule skipped.");
			continue;
		}

		if (line[1].empty())
		{
			Log(this).error("Failed to load molecule due to empty SMILES string.");
			continue;
		}

		auto structure = DataHelpers::parse<MolecularStructure>(line[1]);
		if (structure.has_value() == false)
		{
			Log(this).error("Invalid SMILES.");
			continue;
		}

		if (line[2].empty())
		{
			Log(this).warn("Empty molecule name at {0}.", line[1]);
		}

		const auto id = DataHelpers::parseId<MoleculeId>(line[0]);
		if (id.has_value() == false)
		{
			Log(this).error("Missing id, molecule '{0}' skipped.", line[1]);
			continue;
		}

		// boiling and melting points
		const auto mpR = DataHelpers::parse<double>(line[3]);
		const auto bpR = DataHelpers::parse<double>(line[4]);
		const auto& mpA = estimators.add<OffsetEstimator>(estimators.at(toId(BuiltinEstimator::TORR_TO_REL_BP)), mpR.value_or(0));
		const auto& bpA = estimators.add<OffsetEstimator>(estimators.at(toId(BuiltinEstimator::TORR_TO_REL_BP)), bpR.value_or(100));

		// densities
		const auto sdR = DataHelpers::parse<Spline<float>>(line[5]);
		const auto ldR = DataHelpers::parse<Spline<float>>(line[6]);
		const auto& sdA = estimators.add<SplineEstimator>(sdR.value_or(Spline<float>({ {0, 1.0} })));
		const auto& ldA = estimators.add<SplineEstimator>(ldR.value_or(Spline<float>({ {0, 1.0} })));

		// heat capacities
		const auto shcR = DataHelpers::parse<Spline<float>>(line[7]);
		const auto lhcR = DataHelpers::parse<Spline<float>>(line[8]);
		const auto& shcA = estimators.add<SplineEstimator>(shcR.value_or(Spline<float>({ {36.0, 760.0} })));
		const auto& lhcA = estimators.add<SplineEstimator>(lhcR.value_or(Spline<float>({ {75.4840232, 760.0} })));

		// latent heats
		const auto flhR = DataHelpers::parse<double>(line[9]);
		const auto vlhR = DataHelpers::parse<double>(line[10]);
		const auto slhR = DataHelpers::parse<double>(line[11]);
		const auto& flhA = estimators.add<ScaleEstimator>(estimators.at(toId(BuiltinEstimator::TDIF_TORR_TO_REL_LH)), flhR.value_or(6020.0));
		const auto& vlhA = estimators.add<ScaleEstimator>(estimators.at(toId(BuiltinEstimator::TDIF_TORR_TO_REL_LH)), vlhR.value_or(40700.0));
		const auto& slhA = estimators.add<ScaleEstimator>(estimators.at(toId(BuiltinEstimator::TDIF_TORR_TO_REL_LH)), slhR.value_or(std::numeric_limits<double>::max()));

		//solubility
		const auto hydro = DataHelpers::parseUnsigned<Unit::MOLE_RATIO>(line[12]).value_or(1.0);
		const auto lipo = DataHelpers::parseUnsigned<Unit::MOLE_RATIO>(line[13]).value_or(0.0);
		const auto solR = DataHelpers::parse<Spline<float>>(line[14]);
		const auto& solA = 
			solR.has_value() ? estimators.add<SplineEstimator>(*solR) :
			DataHelpers::parse<bool>(line[14]).value_or(false) ? estimators.at(toId(BuiltinEstimator::TEMP_TO_REL_INV_SOL)) :
			estimators.at(toId(BuiltinEstimator::TEMP_TO_REL_SOL));
		const auto& henryR = DataHelpers::parse<Spline<float>>(line[15]);
		const auto& henryA = estimators.add<SplineEstimator>(henryR.value_or(Spline<float>({ {1000.0, 760.0} })));
			
		//color
		const auto color = DataHelpers::parse<Color>(line[16]).value_or(Color(0, 255, 255, 100));

		if (table.emplace(
			*id,
			line[1],
			MoleculeData(*id, line[2], std::move(*structure), hydro, lipo, color, mpA, bpA, sdA, ldA, shcA, lhcA, flhA, vlhA, slhA, solA, henryA)
		) == false)
		{
			Log(this).warn("Molecule with duplicate id {0} skipped.", *id);
		}
	}
	file.close();

	Log(this).info("Loaded {0} molecules.", table.size());

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
