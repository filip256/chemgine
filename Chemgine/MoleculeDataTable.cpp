#include "MoleculeDataTable.hpp"
#include "DataHelpers.hpp"
#include "OffsetApproximator.hpp"
#include "ScaleApproximator.hpp"
#include "SplineApproximator.hpp"
#include "Logger.hpp"

#include <fstream>

MoleculeDataTable::MoleculeDataTable(ApproximatorDataTable& approximators) noexcept :
	approximators(approximators)
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

		if (line.size() != 12)
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

		const auto id = DataHelpers::toUInt(line[0]);

		if (id.has_value() == false)
		{
			Logger::log("Missing id, molecule '" + line[1] + "' skipped.", LogType::BAD);
			continue;
		}

		// boiling and melting points
		const auto mpR = DataHelpers::toDouble(line[3]);
		const auto bpR = DataHelpers::toDouble(line[4]);
		const auto& mpA = approximators.add<OffsetApproximator>(approximators.at(static_cast<ApproximatorIdType>(Approximators::TORR_TO_REL_BP)), mpR.value_or(0));
		const auto& bpA = approximators.add<OffsetApproximator>(approximators.at(static_cast<ApproximatorIdType>(Approximators::TORR_TO_REL_BP)), bpR.value_or(100));

		// densities
		const auto sdR = DataHelpers::toSpline(line[5]);
		const auto ldR = DataHelpers::toSpline(line[6]);
		const auto& sdA = approximators.add<SplineApproximator>(sdR.value_or(Spline<float>({ {0, 1.0f} })));
		const auto& ldA = approximators.add<SplineApproximator>(ldR.value_or(Spline<float>({ {0, 1.0f} })));

		// heat capacities
		const auto shcR = DataHelpers::toSpline(line[7]);
		const auto lhcR = DataHelpers::toSpline(line[8]);
		const auto& shcA = approximators.add<SplineApproximator>(shcR.value_or(Spline<float>({ {0, 36.0f} })));
		const auto& lhcA = approximators.add<SplineApproximator>(lhcR.value_or(Spline<float>({ {40.0f, 75.24f} })));

		// latent heats
		const auto flhR = DataHelpers::toDouble(line[9]);
		const auto vlhR = DataHelpers::toDouble(line[10]);
		const auto slhR = DataHelpers::toDouble(line[11]);
		const auto& flhA = approximators.add<ScaleApproximator>(approximators.at(static_cast<ApproximatorIdType>(Approximators::TDIF_TORR_TO_REL_LH)), flhR.value_or(6020.0f));
		const auto& vlhA = approximators.add<ScaleApproximator>(approximators.at(static_cast<ApproximatorIdType>(Approximators::TDIF_TORR_TO_REL_LH)), vlhR.value_or(40700.0f));
		const auto& slhA = approximators.add<ScaleApproximator>(approximators.at(static_cast<ApproximatorIdType>(Approximators::TDIF_TORR_TO_REL_LH)), slhR.value_or(std::numeric_limits<double>::max()));

		if (table.emplace(
			id.value(),
			line[1],
			std::move(MoleculeData(id.value(), line[2], line[1], mpA, bpA, sdA, ldA, shcA, lhcA, flhA, vlhA, slhA))
		) == false)
		{
			Logger::log("Molecule with duplicate id " + std::to_string(id.value()) + " skipped.", LogType::WARN);
		}
	}
	file.close();

	Logger::log("Loaded " + std::to_string(table.size()) + " molecules.", LogType::GOOD);

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
		file << e.id << ',' << e.getStructure().serialize() << ',' << e.name << ',' << ',' << ',' << ',' << ',' << ',' << ',' << ',' << ',' << '\n';
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

MoleculeIdType MoleculeDataTable::findOrAdd(MolecularStructure&& structure)
{
	if (structure.isEmpty())
		return 0;

	const auto idx = findFirst(structure);
	if (idx != npos)
		return table[idx].id;

	const auto& mpA = approximators.add<OffsetApproximator>(approximators.at(static_cast<ApproximatorIdType>(Approximators::TORR_TO_REL_BP)), 0);
	const auto& bpA = approximators.add<OffsetApproximator>(approximators.at(static_cast<ApproximatorIdType>(Approximators::TORR_TO_REL_BP)), 100);
	const auto& sdA = approximators.add<SplineApproximator>(Spline<float>({ {0, 1.0f} }));
	const auto& ldA = approximators.add<SplineApproximator>(Spline<float>({ {0, 1.0f} }));
	const auto& shcA = approximators.add<SplineApproximator>(Spline<float>({ {0, 36.0f} }));
	const auto& lhcA = approximators.add<SplineApproximator>(Spline<float>({ {40.0f, 75.24f} }));
	const auto& flhA = approximators.add<ScaleApproximator>(approximators.at(static_cast<ApproximatorIdType>(Approximators::TDIF_TORR_TO_REL_LH)), 6020.0f);
	const auto& vlhA = approximators.add<ScaleApproximator>(approximators.at(static_cast<ApproximatorIdType>(Approximators::TDIF_TORR_TO_REL_LH)), 40700.0f);
	const auto& slhA = approximators.add<ScaleApproximator>(approximators.at(static_cast<ApproximatorIdType>(Approximators::TDIF_TORR_TO_REL_LH)), std::numeric_limits<double>::max());

	const auto id = getFreeId();
	table.emplace(id, std::to_string(id), MoleculeData(id, std::move(structure), mpA, bpA, sdA, ldA, shcA, lhcA, flhA, vlhA, slhA));
	return id;
}