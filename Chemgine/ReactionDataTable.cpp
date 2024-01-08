#include "ReactionDataTable.hpp"
#include "DataHelpers.hpp"
#include "Logger.hpp"
#include "Molecule.hpp"

#include <fstream>

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
		if (buffer.starts_with('#'))
			continue;

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

		std::vector<std::pair<Reactable, uint8_t>>reactantIds;
		reactantIds.reserve(reactants.size());
		for (size_t i = 0; i < reactants.size(); ++i)
		{
			const auto r = Reactable::get(reactants[i]);
			if (r.getId() == 0)
			{
				Logger::log("Undefined reactant '" + reactants[i] + "' in reaction with id " + std::to_string(id.result) + " skipped.", LogType::BAD);
				continue; // TODO: add unknown into molecules and predict properties
			}
			reactantIds.emplace_back(std::make_pair(r, 0));
		}

		std::vector<std::pair<Reactable, uint8_t>>productIds;
		productIds.reserve(products.size());
		for (size_t i = 0; i < products.size(); ++i)
		{
			const auto r = Reactable::get(products[i]);
			if (r.getId() == 0)
			{
				Logger::log("Undefined product '" + products[i] + "' in reaction with id " + std::to_string(id.result) + " skipped.", LogType::BAD);
				continue; // TODO: add unknown into molecules and predict properties
			}
			productIds.emplace_back(std::make_pair(r, 0));
		}

		if (ReactionData::balance(reactantIds, productIds) == false)
		{
			Logger::log("Reaction with id " + std::to_string(id.result) + " could not be balanced.", LogType::BAD);
			continue;
		}

		// speed
		const auto speed = DataHelpers::toValueAtTemperature<Unit::MOLE_PER_SECOND>(line[5]);	
		if(speed.status == 0)
		{
			Logger::log("Reaction speed for the reaction with id " + std::to_string(id.result) + " is ill-defined. Skipped.", LogType::BAD);
			continue;
		}

		// create
		ReactionData data(id.result, line[1], reactantIds, productIds, speed.result.first, speed.result.second);
		if (data.mapReactantsToProducts() == false)
		{
			Logger::log("Reaction with id " + std::to_string(id.result) + " is not a valid reaction.", LogType::BAD);
			continue;
		}

		if (table.emplace(
			id.result,
			std::to_string(id.result),
			std::move(data)
		) == false)
		{
			Logger::log("Reaction with duplicate id " + std::to_string(id.result) + " skipped.", LogType::WARN);
		}
	}
	file.close();

	Logger::log("Loaded " + std::to_string(table.size()) + " reactions.", LogType::GOOD);

	return true;
}

std::unordered_set<ConcreteReaction, ConcreteReactionHash> ReactionDataTable::findOccuringReactions(const std::vector<Reactant>& reactants) const
{
	std::unordered_set<ConcreteReaction, ConcreteReactionHash> result;
	for (size_t i = 0; i < table.size(); ++i)
	{
		const auto& products = table[i].generateConcreteProducts(reactants);
		if (products.size())
			result.insert(ConcreteReaction(table[i], reactants, products));
	}
	return result;
}