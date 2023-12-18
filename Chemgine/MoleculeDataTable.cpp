#include "MoleculeDataTable.hpp"
#include "DataHelpers.hpp"
#include "OffsetApproximator.hpp"
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

		if (line.size() != 7)
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

		if (id.status == 0)
		{
			Logger::log("Missing id, molecule '" + line[1] + "' skipped.", LogType::BAD);
			continue;
		}

		const auto& x = approximators.at(static_cast<ApproximatorIdType>(Approximators::TORR_TO_REL_BP));

		// boiling and melting points
		const auto mpR = DataHelpers::toDouble(line[3]);
		const auto bpR = DataHelpers::toDouble(line[4]);
		const double mp = mpR.status == 0 ? 0 : mpR.result;
		const double bp = bpR.status == 0 ? 100 : bpR.result;
		const auto& mpA = approximators.add(OffsetApproximator(0, "", approximators.at(static_cast<ApproximatorIdType>(Approximators::TORR_TO_REL_BP)), mp));
		const auto& bpA = approximators.add(OffsetApproximator(0, "", approximators.at(static_cast<ApproximatorIdType>(Approximators::TORR_TO_REL_BP)), bp));

		// densities
		const auto sdR = DataHelpers::toSpline(line[5]);
		const auto ldR = DataHelpers::toSpline(line[6]);
		Spline<float> sdS = sdR.status == 0 ? Spline<float>({ {0, 1.0f} }) : sdR.result;
		Spline<float> ldS = ldR.status == 0 ? Spline<float>({ {0, 1.0f} }) : ldR.result;
		const auto& sdA = approximators.add(SplineApproximator(0, "", std::move(sdS)));
		const auto& ldA = approximators.add(SplineApproximator(0, "", std::move(ldS)));


		if (table.emplace(
			id.result,
			line[1],
			std::move(MoleculeData(id.result, line[2], line[1], mpA, bpA, sdA, ldA))
		) == false)
		{
			Logger::log("Molecule with duplicate id " + std::to_string(id.result) + " skipped.", LogType::WARN);
		}
	}
	file.close();

	Logger::log("Loaded " + std::to_string(table.size()) + " molecules.", LogType::GOOD);

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
	const auto idx = findFirst(structure);
	if (idx != npos)
		return table[idx].id;

	const auto& mpA = approximators.add(OffsetApproximator(0, "", approximators.at(static_cast<ApproximatorIdType>(Approximators::TORR_TO_REL_BP)), 0));
	const auto& bpA = approximators.add(OffsetApproximator(0, "", approximators.at(static_cast<ApproximatorIdType>(Approximators::TORR_TO_REL_BP)), 100));
	const auto& sdA = approximators.add(SplineApproximator(0, "", Spline<float>({ {0, 1.0f} })));
	const auto& ldA = approximators.add(SplineApproximator(0, "", Spline<float>({ {0, 1.0f} })));

	const auto id = getFreeId();
	table.emplace(id, std::to_string(id), MoleculeData(id, std::move(structure), mpA, bpA, sdA, ldA));
	return id;
}