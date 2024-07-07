#include "ReactionRepository.hpp"
#include "DataHelpers.hpp"
#include "Log.hpp"
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
		if (buffer.starts_with('\\'))
			continue;

		auto line = DataHelpers::parseList(buffer, ',');

		if (line.size() != 8)
		{
			Log(this).error("Incompletely defined reaction skipped.");
			continue;
		}

		const auto id = DataHelpers::parseId<ReactionId>(line[0]);
		if (id.has_value() == false)
		{
			Log(this).error("Missing id, reaction skipped.");
			continue;
		}

		// reactants
		const auto reactants = DataHelpers::parseList(line[2], ';', true);
		if (reactants.empty())
		{
			Log(this).error("Reaction without reactants with id {0} skipped.", *id);
			continue;
		}

		std::vector<std::pair<Reactable, uint8_t>> reactantIds;
		reactantIds.reserve(reactants.size());
		for (size_t i = 0; i < reactants.size(); ++i)
		{
			const auto r = Reactable::get(reactants[i]);
			if (r.has_value() == false)
			{
				Log(this).error("Ill-defined reactant '{0}' in reaction with id {1} skipped.", reactants[i], *id);
				continue;
			}
			reactantIds.emplace_back(std::make_pair(*r, 0));
		}

		// products
		const auto products = DataHelpers::parseList(line[3], ';', true);
		if (products.empty())
		{
			Log(this).error("Reaction without products with id {0} skipped.", *id);
			continue;
		}

		std::vector<std::pair<Reactable, uint8_t>> productIds;
		productIds.reserve(products.size());
		for (size_t i = 0; i < products.size(); ++i)
		{
			const auto r = Reactable::get(products[i]);
			if (r.has_value() == false)
			{
				Log(this).error("Ill-defined product '{0}' in reaction with id {1} skipped.", products[i], *id);
				continue;
			}
			productIds.emplace_back(std::make_pair(*r, 0));
		}

		if (ReactionData::balance(reactantIds, productIds) == false)
		{
			Log(this).error("Reaction with id {0} could not be balanced.", *id);
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
				Log(this).error("Reaction catalyst for the reaction with id {0} is ill-defined. Catalysts skipped.", *id);
				continue;
			}

			const auto amount = DataHelpers::parseUnsigned<Unit::MOLE_RATIO>(pair.back());
			if (amount.has_value() == false)
			{
				Log(this).error("Ill-defined catalyst '{0}' in reaction with id {1} skipped.", catStr[i], *id);
				continue;
			}

			const auto c = Catalyst::get(pair.front(), *amount);
			if (c.has_value() == false)
			{
				Log(this).error("Ill-defined catalyst '{0}' in reaction with id {1} skipped.", catStr[i], *id);
				continue;
			}

			bool isUnique = true;
			for(size_t j = 0; j < catalysts.size(); ++j)
				if (catalysts[j].matchesWith(c->getStructure()) || c->matchesWith(catalysts[j].getStructure()))
				{
					isUnique = false;
					Log(this).warn("Ingored duplicate catalyst '{0}' in reaction with id {1}.", catStr[i], *id);
					break;
				}
			if (isUnique == false)
				continue;

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
			Log(this).error("Reaction with id {0} is not a valid reaction.", *id);
			continue;
		}

		maxReactantCount = std::max(static_cast<uint8_t>(reactantIds.size()), maxReactantCount);

		if (table.emplace(
			*id,
			std::to_string(*id),
			std::move(data)
		) == false)
		{
			Log(this).warn("Reaction with duplicate id {0} skipped.", *id);
		}
	}
	file.close();

	Log(this).info("Loaded {0} reactions.", table.size());

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

	while (generateCurrentSpan() > 0 && molecules.getData().getData().size() < 1000);

	return molecules.getData().getData().size() - initialCnt;
}
