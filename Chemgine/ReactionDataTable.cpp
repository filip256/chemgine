#include "ReactionDataTable.hpp"
#include "DataHelpers.hpp"
#include "Logger.hpp"
#include "ReactableFactory.hpp"

#include <fstream>

ReactionDataTable::ReactionDataTable(
	const FunctionalGroupDataTable& functionalGroups,
	const BackboneDataTable& backbones,
	const MoleculeDataTable& molecules
) noexcept :
	functionalGroups(functionalGroups),
	backbones(backbones),
	molecules(molecules)
{}

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

		const ReactableFactory factory;
		std::vector<std::pair<const Reactable*, uint8_t>>reactantIds;
		reactantIds.reserve(reactants.size());
		for (size_t i = 0; i < reactants.size(); ++i)
		{
			const auto r = factory.get(reactants[i]);
			if (r == nullptr)
			{
				Logger::log("Undefined reactant '" + reactants[i] + "' in reaction with id " + std::to_string(id.result) + " skipped.", LogType::BAD);
				continue;
			}
			reactantIds.emplace_back(std::make_pair(r, 0));
		}

		std::vector<std::pair<const Reactable*, uint8_t>>productIds;
		productIds.reserve(products.size());
		for (size_t i = 0; i < products.size(); ++i)
		{
			const auto r = factory.get(products[i]);
			if (r == 0)
			{
				Logger::log("Undefined product '" + products[i] + "' in reaction with id " + std::to_string(id.result) + " skipped.", LogType::BAD);
				continue;
			}
			productIds.emplace_back(std::make_pair(r, 0));
		}

		auto temp = ReactionData(id.result, line[1], std::move(reactantIds), std::move(productIds));
		if (temp.balance() == false)
		{
			Logger::log("Reaction with id " + std::to_string(id.result) + " could not be balanced.", LogType::BAD);
			continue;
		}

		if (table.emplace(
			id.result,
			line[1],
			std::move(temp)
		) == false)
		{
			Logger::log("Reaction with duplicate id " + std::to_string(id.result) + " skipped.", LogType::WARN);
		}
	}
	file.close();

	Logger::log("Loaded " + std::to_string(table.size()) + " reactions.", LogType::GOOD);

	return true;
}

