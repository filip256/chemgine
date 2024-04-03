#include "ReactionRepository.hpp"
#include "DataHelpers.hpp"
#include "Logger.hpp"
#include "Molecule.hpp"

#include <fstream>

bool ReactionRepository::loadFromFile(const std::string& path)
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
		if (buffer.starts_with('\\'))
			continue;

		auto line = DataHelpers::parseList(buffer, ',');

		if (line.size() != 8)
		{
			Logger::log("Incompletely defined reaction skipped.", LogType::BAD);
			continue;
		}

		const auto id = DataHelpers::parseId<ReactionId>(line[0]);
		if (id.has_value() == false)
		{
			Logger::log("Missing id, reaction skipped.", LogType::BAD);
			continue;
		}

		// reactants
		const auto reactants = DataHelpers::parseList(line[2], ';', true);
		if (reactants.empty())
		{
			Logger::log("Reaction without reactants with id " + std::to_string(*id) + " skipped.", LogType::BAD);
			continue;
		}

		std::vector<std::pair<Reactable, uint8_t>> reactantIds;
		reactantIds.reserve(reactants.size());
		for (size_t i = 0; i < reactants.size(); ++i)
		{
			const auto r = Reactable::get(reactants[i]);
			if (r.has_value() == false)
			{
				Logger::log("Ill-defined reactant '" + reactants[i] + "' in reaction with id " + std::to_string(*id) + " skipped.", LogType::BAD);
				continue;
			}
			reactantIds.emplace_back(std::make_pair(*r, 0));
		}

		// products
		const auto products = DataHelpers::parseList(line[3], ';', true);
		if (products.empty())
		{
			Logger::log("Reaction without products with id " + std::to_string(*id) + " skipped.", LogType::BAD);
			continue;
		}

		std::vector<std::pair<Reactable, uint8_t>> productIds;
		productIds.reserve(products.size());
		for (size_t i = 0; i < products.size(); ++i)
		{
			const auto r = Reactable::get(products[i]);
			if (r.has_value() == false)
			{
				Logger::log("Ill-defined product '" + products[i] + "' in reaction with id " + std::to_string(*id) + " skipped.", LogType::BAD);
				continue;
			}
			productIds.emplace_back(std::make_pair(*r, 0));
		}

		if (ReactionData::balance(reactantIds, productIds) == false)
		{
			Logger::log("Reaction with id " + std::to_string(*id) + " could not be balanced.", LogType::BAD);
			continue;
		}

		// speed
		const auto speed = DataHelpers::parsePair<Unit::MOLE_PER_SECOND, Unit::CELSIUS>(line[4])
			.value_or(std::make_pair(1.0, 20.0));

		//reaction energy
		const auto reactionEnergy = DataHelpers::parse<double>(line[5]);

		//activation energy
		const auto activationEnergy = DataHelpers::parseUnsigned<double>(line[6]);

		//catalysts
		const auto catStr = DataHelpers::parseLists(line[7], ';', '|', true);
		std::vector<std::vector<Catalyst>> catalysts;
		catalysts.reserve(catStr.size());
		for (size_t i = 0; i < catStr.size(); ++i)
		{
			catalysts.emplace_back();
			catalysts.back().reserve(catStr[i].size());
			for (size_t j = 0; j < catStr[i].size(); ++j)
			{
				const auto pair = DataHelpers::parseList(catStr[i][j], '@', true);
				if (pair.size() != 2)
				{
					Logger::log("Reaction catalyst for the reaction with id " + std::to_string(*id) + " is ill-defined. Catalysts skipped.", LogType::BAD);
					continue;
				}

				const auto amount = DataHelpers::parseUnsigned<double>(pair.back());
				if (amount.has_value() == false)
				{
					Logger::log("Ill-defined catalyst '" + products[i] + "' in reaction with id " + std::to_string(*id) + " skipped.", LogType::BAD);
					continue;
				}

				const auto c = Catalyst::get(pair.front(), *amount);
				if (c.has_value() == false)
				{
					Logger::log("Ill-defined catalyst '" + products[i] + "' in reaction with id " + std::to_string(*id) + " skipped.", LogType::BAD);
					continue;
				}

				catalysts.back().emplace_back(*c);
			}
		}

		// create
		ReactionData data(
			*id, line[1],
			reactantIds, productIds, 
			speed.first, speed.second,
			reactionEnergy.value_or(0.0),
			activationEnergy.value_or(0.0),
			std::move(catalysts)
		);

		if (data.mapReactantsToProducts() == false)
		{
			Logger::log("Reaction with id " + std::to_string(*id) + " is not a valid reaction.", LogType::BAD);
			continue;
		}

		if (table.emplace(
			*id,
			std::to_string(*id),
			std::move(data)
		) == false)
		{
			Logger::log("Reaction with duplicate id " + std::to_string(*id) + " skipped.", LogType::WARN);
		}
	}
	file.close();

	Logger::log("Loaded " + std::to_string(table.size()) + " reactions.", LogType::INFO);

	for (size_t i = 0; i < table.size(); ++i)
		network.insert(table[i]);

	return true;
}

const ReactionNetwork& ReactionRepository::getNetwork() const
{
	return network;
}

std::unordered_set<ConcreteReaction> ReactionRepository::findOccuringReactions(const std::vector<Reactant>& reactants) const
{
	return network.getConcreteReactions(reactants);
}
