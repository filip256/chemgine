#include "ReactionDataTable.hpp"
#include "DataHelpers.hpp"
#include "Logger.hpp"

#include <fstream>
#include <algorithm>

ReactionDataTable::ReactionDataTable(
	const FunctionalGroupDataTable& functionalGroups,
	const BackboneDataTable& backbones,
	const MoleculeDataTable& molecules
) noexcept :
	functionalGroups(functionalGroups),
	backbones(backbones),
	molecules(molecules)
{}

ComponentIdType ReactionDataTable::findComponent(const std::string& smiles) const
{
	size_t result = functionalGroups.findFirst(smiles);
	if (result != DataTable::npos)
		return functionalGroups[result].id;

	result = backbones.findFirst(smiles);
	if (result != DataTable::npos)
		return backbones[result].id;

	return 0;
}

std::vector<ComponentIdType> ReactionDataTable::findComponents(const std::vector<std::string>& smilesList) const
{
	std::vector<ComponentIdType> result;
	result.reserve(smilesList.size());
	for (size_t i = 0; i < smilesList.size(); ++i)
		result.emplace_back(findComponent(smilesList[i]));
	return result;
}

bool ReactionDataTable::loadFromFile(const std::string& path)
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

		const auto id = DataHelpers::toUInt(line[0]);
		if (id.status == 0)
		{
			Logger::log("Missing id, reaction skipped.", LogType::BAD);
			continue;
		}

		const auto reactants = DataHelpers::parseList(line[2], ';', true);
		if (reactants.empty())
		{
			Logger::log("Reaction without reactants with id " + std::to_string(id.result) + " skipped.", LogType::BAD);
			continue;
		}

		const auto products = DataHelpers::parseList(line[3], ';', true);
		if (products.empty())
		{
			Logger::log("Reaction without products with id " + std::to_string(id.result) + " skipped.", LogType::BAD);
			continue;
		}

		auto reactantsIds = findComponents(reactants);
		if (std::any_of(reactantsIds.begin(), reactantsIds.end(), [](ComponentIdType id) {return id == 0; }))
		{
			Logger::log("Undefined reactant, reaction with id " + std::to_string(id.result) + " skipped.", LogType::BAD);
			continue;
		}

		auto productsIds = findComponents(products);
		if (std::any_of(reactantsIds.begin(), reactantsIds.end(), [](ComponentIdType id) {return id == 0; }))
		{
			Logger::log("Undefined product, reaction with id " + std::to_string(id.result) + " skipped.", LogType::BAD);
			continue;
		}


		if (table.emplace(
			id.result,
			line[1],
			std::move(ReactionData(id.result, line[1], std::move(reactantsIds), std::move(productsIds)))
		) == false)
		{
			Logger::log("Reaction with duplicate id " + std::to_string(id.result) + " skipped.", LogType::WARN);
		}
	}
	file.close();

	Logger::log("Loaded " + std::to_string(table.size()) + " reactions.", LogType::GOOD);

	return true;
}

