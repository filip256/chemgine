#include "MoleculeDataTable.hpp"
#include "DataHelpers.hpp"
#include "OffsetEstimator.hpp"
#include "ScaleEstimator.hpp"
#include "SplineEstimator.hpp"
#include "Logger.hpp"

#include <fstream>

MoleculeDataTable::MoleculeDataTable(EstimatorDataTable& estimators) noexcept :
	estimators(estimators)
{}


bool MoleculeDataTable::loadFromFile(const std::string& path)
{
	std::ifstream file(path);

	if (!file.is_open())
	{
		Logger::log("Failed to open file '" + path + "'.", LogType::BAD);
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
		auto line = DataHelpers::parseList(buffer, ',');

		if (line.size() != 17)
		{
			Logger::log("Incompletely defined molecule skipped.", LogType::BAD);
			continue;
		}

		if (line[1].empty())
		{
			Logger::log("Failed to load molecule due to empty SMILES string.", LogType::BAD);
			continue;
		}
		if (line[2].empty())
		{
			Logger::log("Molecule name was empty. (" + line[1] + ')', LogType::WARN);
		}

		const auto id = DataHelpers::parse<unsigned int>(line[0]);

		if (id.has_value() == false)
		{
			Logger::log("Missing id, molecule '" + line[1] + "' skipped.", LogType::BAD);
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
			MoleculeData(*id, line[2], line[1], hydro, lipo, color, mpA, bpA, sdA, ldA, shcA, lhcA, flhA, vlhA, slhA, solA, henryA)
		) == false)
		{
			Logger::log("Molecule with duplicate id " + std::to_string(*id) + " skipped.", LogType::WARN);
		}
	}
	file.close();

	Logger::log("Loaded " + std::to_string(table.size()) + " molecules.", LogType::INFO);

	return true;
}

bool MoleculeDataTable::saveToFile(const std::string& path)
{
	std::ofstream file(path);

	if (!file.is_open())
	{
		Logger::log("Failed to open file '" + path + "'.", LogType::BAD);
		return false;
	}

	for (size_t i = 0; i < table.size(); ++i)
	{
		const auto& e = table[i];
		file << e.id << ',' << e.getStructure().serialize() << ',' << e.name << ',' << ',' << ',' << ',' << ',' << ',' << ',' << ',' << ',' << ',' << ',' << ',' << ',' << ',' << '\n';
	}

	file.close();
	return true;
}

size_t MoleculeDataTable::findFirst(const MolecularStructure& structure) const
{
	for (size_t i = 0; i < table.size(); ++i)
		if (table[i].getStructure() == structure)
			return i;

	return npos;
}

MoleculeId MoleculeDataTable::findOrAdd(MolecularStructure&& structure)
{
	if (structure.isEmpty())
		return 0;

	const auto idx = findFirst(structure);
	if (idx != npos)
		return table[idx].id;

	Logger::log("New structure discovered: \n" + structure.print(50, 10), LogType::INFO);

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