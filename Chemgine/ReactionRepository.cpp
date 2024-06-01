#include "ReactionRepository.hpp"
#include "DataHelpers.hpp"
#include "Logger.hpp"
#include "Molecule.hpp"

#include <fstream>

ReactionRepository::ReactionRepository(MoleculeRepository& molecules) noexcept :
	molecules(molecules)
{}

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

		//speed
		const auto speed = DataHelpers::parsePair<Unit::MOLE_PER_SECOND, Unit::CELSIUS>(line[4])
			.value_or(std::make_pair(1.0, 20.0));

		//reaction energy
		const auto reactionEnergy = DataHelpers::parse<Unit::JOULE_PER_MOLE>(line[5]);

		//activation energy
		const auto activationEnergy = DataHelpers::parseUnsigned<Unit::JOULE_PER_MOLE>(line[6]);

		//catalysts
		const auto catStr = DataHelpers::parseList(line[7], ';', true);
		std::vector<Catalyst> catalysts;
		catalysts.reserve(catStr.size());
		for (size_t i = 0; i < catStr.size(); ++i)
		{
			const auto pair = DataHelpers::parseList(catStr[i], '@', true);
			if (pair.size() != 2)
			{
				Logger::log("Reaction catalyst for the reaction with id " + std::to_string(*id) + " is ill-defined. Catalysts skipped.", LogType::BAD);
				continue;
			}

			const auto amount = DataHelpers::parseUnsigned<Unit::MOLE_RATIO>(pair.back());
			if (amount.has_value() == false)
			{
				Logger::log("Ill-defined catalyst '" + catStr[i] + "' in reaction with id " + std::to_string(*id) + " skipped.", LogType::BAD);
				continue;
			}

			const auto c = Catalyst::get(pair.front(), *amount);
			if (c.has_value() == false)
			{
				Logger::log("Ill-defined catalyst '" + catStr[i] + "' in reaction with id " + std::to_string(*id) + " skipped.", LogType::BAD);
				continue;
			}

			catalysts.emplace_back(*c);
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

		maxReactantCount = std::max(static_cast<uint8_t>(reactantIds.size()), maxReactantCount);

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

uint8_t ReactionRepository::getMaxReactantCount() const
{
	return maxReactantCount;
}

const ReactionNetwork& ReactionRepository::getNetwork() const
{
	return network;
}

std::unordered_set<ConcreteReaction> ReactionRepository::findOccuringReactions(const std::vector<Reactant>& reactants) const
{
	return network.getOccuringReactions(reactants);
}

std::unordered_set<RetrosynthReaction> ReactionRepository::getRetrosynthReactions(const Reactable& targetProduct) const
{
	return network.getRetrosynthReactions(targetProduct);
}

size_t ReactionRepository::generateCurrentSpan() const
{
	const auto& molecules = this->molecules.getData().getData();
	std::vector<Reactant> reactants;
	reactants.reserve(molecules.size());

	std::transform(
		molecules.begin(), molecules.end(), std::back_inserter(reactants),
		[](const MoleculeData& m) { return Reactant(Molecule(m.id), LayerType::NONE, 1.0_mol); });

	const auto arrangements = Utils::getArrangementsWithRepetitions(reactants, maxReactantCount);
	for(size_t i = 0; i < arrangements.size(); ++i)
	{
		findOccuringReactions(arrangements[i]);
	}

	return molecules.size() - reactants.size();
}

size_t ReactionRepository::generateTotalSpan(const size_t maxIterations) const
{
	const size_t initialCnt = molecules.getData().getData().size();

	while (generateCurrentSpan() > 0);

	return molecules.getData().getData().size() - initialCnt;
}
