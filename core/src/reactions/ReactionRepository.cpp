#include "reactions/ReactionRepository.hpp"
#include "reactions/ReactionSpecifier.hpp"
#include "estimators/EstimatorParsers.hpp"
#include "molecules/kinds/Molecule.hpp"
#include "data/def/Keywords.hpp"
#include "io/Log.hpp"

#include <fstream>

ReactionRepository::ReactionRepository(
	EstimatorRepository& estimators,
	const MoleculeRepository& molecules
) noexcept :
	estimators(estimators),
	molecules(molecules)
{}

bool ReactionRepository::add(const def::Object& definition)
{
	auto id = definition.getOptionalProperty(def::Reactions::Id, def::parse<ReactionId>);
	if (not id)
		id = getFreeId();
	else if (reactions.contains(*id))
	{
		Log(this).error("Reaction with duplicate id: '{0}', at: {1}.", *id, definition.getLocationName());
		return false;
	}

	const auto name = definition.getDefaultProperty("name", "?");

	const auto spec = def::parse<def::ReactionSpecifier>(definition.getSpecifier());
	if (not spec)
	{
		Log(this).error("Invalid reaction specifier: '{0}', at: {1}.", definition.getSpecifier(), definition.getLocationName());
		return false;
	}

	// reactants
	std::vector<std::pair<StructureRef, uint8_t>> reactantIds;
	reactantIds.reserve(spec->reactants.size());
	for (size_t i = 0; i < spec->reactants.size(); ++i)
	{
		const auto r = StructureRef::create(spec->reactants[i]);
		if (not r)
		{
			Log(this).error("Malformed reactant: '{0}' in reaction with id: {1}, at: {2}.", spec->reactants[i], *id, definition.getLocationName());
			return false;
		}
		reactantIds.emplace_back(std::make_pair(*r, 0));
	}

	// products
	std::vector<std::pair<StructureRef, uint8_t>> productIds;
	productIds.reserve(spec->products.size());
	for (size_t i = 0; i < spec->products.size(); ++i)
	{
		const auto p = StructureRef::create(spec->products[i]);
		if (not p)
		{
			Log(this).error("Malformed product: '{0}' in reaction with id: {1}, at: {2}.", spec->products[i], *id, definition.getLocationName());
			return false;
		}
		productIds.emplace_back(std::make_pair(*p, 0));
	}

	if (ReactionData::balance(reactantIds, productIds) == false)
	{
		Log(this).error("Reaction with id: '{0}' could not be balanced.", *id);
		return false;
	}

	// catalysts
	const auto catStr = definition.getDefaultProperty(def::Reactions::Catalysts, std::vector<std::string>(),
		def::parse<std::vector<std::string>>);

	std::vector<Catalyst> catalysts;
	catalysts.reserve(catStr.size());
	for (size_t i = 0; i < catStr.size(); ++i)
	{
		const auto c = def::parse<Catalyst>(catStr[i]);
		if (not c)
		{
			Log(this).error("Malformed catalyst: '{0}' in reaction with id: '{1}', at: {2}.", catStr[i], *id, definition.getLocationName());
			return false;
		}

		bool isUnique = true;
		for (size_t j = 0; j < catalysts.size(); ++j)
			if (catalysts[j].matchesWith(c->getStructure()) || c->matchesWith(catalysts[j].getStructure()))
			{
				isUnique = false;
				Log(this).warn("Ignored duplicate catalyst '{0}' in reaction with id {1}, at: {2}.", catStr[i], *id, definition.getLocationName());
				break;
			}
		if (isUnique == false)
			continue;

		catalysts.emplace_back(*c);
	}

	const auto isCut = definition.getDefaultProperty(def::Reactions::IsCut, false,
		def::parse<bool>);

	std::unique_ptr<ReactionData> data;
	if (isCut)
	{
		EstimatorFactory factory(estimators);
		data = std::make_unique<ReactionData>(
			*id, name,
			reactantIds, productIds,
			factory.createConstant<Unit::MOLE_PER_SECOND, Unit::CELSIUS>(0.0),
			factory.createConstant<Unit::NONE, Unit::MOLE_RATIO>(0.0),
			std::move(catalysts));
	}
	else
	{
		const auto energy = definition.getDefaultProperty(def::Reactions::Energy, Amount<Unit::JOULE_PER_MOLE>(0.0),
			def::parse<Amount<Unit::JOULE_PER_MOLE>>);
		const auto activation = definition.getDefaultProperty(def::Reactions::Activation, Amount<Unit::JOULE_PER_MOLE>(0.0),
			def::parse<Amount<Unit::JOULE_PER_MOLE>>);
		auto tempSpeed = definition.getDefinition(def::Reactions::TemperatureSpeed,
			def::Parser<UnitizedEstimator<Unit::MOLE_PER_SECOND, Unit::CELSIUS>>::parse, estimators);
		auto concSpeed = definition.getDefinition(def::Reactions::ConcentrationSpeed,
			def::Parser<UnitizedEstimator<Unit::NONE, Unit::MOLE_RATIO>>::parse, estimators);

		data = std::make_unique<ReactionData>(
			*id, name,
			reactantIds, productIds,
			energy, activation,
			std::move(*tempSpeed), std::move(*concSpeed),
			std::move(catalysts));
	}

	// create
	if (data->mapReactantsToProducts() == false)
	{
		Log(this).error("Malformed reaction with id: {0}, at: {1}.", *id, definition.getLocationName());
		return false;
	}

	maxReactantCount = std::max(static_cast<uint8_t>(reactantIds.size()), maxReactantCount);

	const auto r = reactions.emplace(*id, std::move(data));
	network.insert(*r.first->second);

	definition.logUnusedWarnings();
	return true;
}

bool ReactionRepository::contains(const ReactionId id) const
{
	return reactions.contains(id);
}

const ReactionData& ReactionRepository::at(const ReactionId id) const
{
	return *reactions.at(id);
}

size_t ReactionRepository::totalDefinitionCount() const
{
	return reactions.size();
}

ReactionRepository::Iterator ReactionRepository::begin() const
{
	return reactions.begin();
}

ReactionRepository::Iterator ReactionRepository::end() const
{
	return reactions.end();
}

size_t ReactionRepository::size() const
{
	return reactions.size();
}

void ReactionRepository::clear()
{
	network.clear();
	reactions.clear();
}

uint8_t ReactionRepository::getMaxReactantCount() const
{
	return maxReactantCount;
}

const ReactionNetwork& ReactionRepository::getNetwork() const
{
	return network;
}

std::unordered_set<ConcreteReaction> ReactionRepository::findOccurringReactions(const std::vector<Reactant>& reactants) const
{
	return network.getOccurringReactions(reactants);
}

std::unordered_set<RetrosynthReaction> ReactionRepository::getRetrosynthReactions(const StructureRef& targetProduct) const
{
	return network.getRetrosynthReactions(targetProduct);
}

size_t ReactionRepository::generateCurrentSpan() const
{
	std::vector<Reactant> reactants;
	reactants.reserve(molecules.size());

	std::transform(
		molecules.begin(), molecules.end(), std::back_inserter(reactants),
		[](const auto& mIt) { return Reactant(Molecule(*mIt.second), LayerType::NONE, 1.0_mol); });

	const auto arrangements = utils::getArrangementsWithRepetitions(reactants, maxReactantCount);
	for(size_t i = 0; i < arrangements.size(); ++i)
	{
		findOccurringReactions(arrangements[i]);
	}

	return molecules.size() - reactants.size();
}

size_t ReactionRepository::generateTotalSpan(const size_t maxIterations) const
{
	const size_t initialCnt = molecules.size();

	while (generateCurrentSpan() > 0 && molecules.size() < 1000);

	return molecules.size() - initialCnt;
}

ReactionId ReactionRepository::getFreeId() const
{
	static ReactionId id = 0;
	while (reactions.contains(id))
	{
		if (id == std::numeric_limits<ReactionId>::max())
			Log(this).fatal("Reaction id limit reached: {0}.", id);
		++id;
	}
	return id;
}
