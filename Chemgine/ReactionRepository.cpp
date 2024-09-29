#include "ReactionRepository.hpp"
#include "ReactionSpecifier.hpp"
#include "EstimatorParsers.hpp"
#include "Molecule.hpp"
#include "Keywords.hpp"
#include "Log.hpp"

#include <fstream>

ReactionRepository::ReactionRepository(
	EstimatorRepository& estimators,
	const MoleculeRepository& molecules
) noexcept :
	estimators(estimators),
	molecules(molecules)
{}

bool ReactionRepository::add(DefinitionObject&& definition)
{
	const auto id = definition.pullProperty("id", Def::parseId<ReactionId>);
	if (not id)
		return false;

	const auto name = definition.pullDefaultProperty("name", "?");

	const auto spec = Def::parse<ReactionSpecifier>(definition.getSpecifier());
	if (not spec)
	{
		Log(this).error("Invalid reaction specifier: '{0}', at: {1}.", definition.getSpecifier(), definition.getLocationName());
		return false;
	}

	// reactants
	std::vector<std::pair<Reactable, uint8_t>> reactantIds;
	reactantIds.reserve(spec->reactants.size());
	for (size_t i = 0; i < spec->reactants.size(); ++i)
	{
		const auto r = Reactable::get(spec->reactants[i]);
		if (not r)
		{
			Log(this).error("Malformed reactant: '{0}' in reaction with id: {1}, at: {2}.", spec->reactants[i], *id, definition.getLocationName());
			return false;
		}
		reactantIds.emplace_back(std::make_pair(*r, 0));
	}

	// products
	std::vector<std::pair<Reactable, uint8_t>> productIds;
	productIds.reserve(spec->products.size());
	for (size_t i = 0; i < spec->products.size(); ++i)
	{
		const auto p = Reactable::get(spec->products[i]);
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
	const auto catStr = definition.pullDefaultProperty(Keywords::Reactions::Catalysts, std::vector<std::string>(),
		Def::parse<std::vector<std::string>>);

	std::vector<Catalyst> catalysts;
	catalysts.reserve(catStr.size());
	for (size_t i = 0; i < catStr.size(); ++i)
	{
		const auto c = Def::parse<Catalyst>(catStr[i]);
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
				Log(this).warn("Ingored duplicate catalyst '{0}' in reaction with id {1}, at: {2}.", catStr[i], *id, definition.getLocationName());
				break;
			}
		if (isUnique == false)
			continue;

		catalysts.emplace_back(*c);
	}

	const auto isCut = definition.pullDefaultProperty(Keywords::Reactions::IsCut, false,
		Def::parse<bool>);

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
		const auto energy = definition.pullDefaultProperty(Keywords::Reactions::Energy, Amount<Unit::JOULE_PER_MOLE>(0.0),
			Def::parse<Amount<Unit::JOULE_PER_MOLE>>);
		const auto activation = definition.pullDefaultProperty(Keywords::Reactions::Activation, Amount<Unit::JOULE_PER_MOLE>(0.0),
			Def::parse<Amount<Unit::JOULE_PER_MOLE>>);
		auto tempSpeed = definition.getDefinition(Keywords::Reactions::TemperatureSpeed,
			Def::Parser<UnitizedEstimator<Unit::MOLE_PER_SECOND, Unit::CELSIUS>>::parse, estimators);
		auto concSpeed = definition.getDefinition(Keywords::Reactions::ConcentrationSpeed,
			Def::Parser<UnitizedEstimator<Unit::NONE, Unit::MOLE_RATIO>>::parse, estimators);

		data = std::make_unique<ReactionData>(
			*id, name,
			reactantIds, productIds,
			energy, activation,
			std::move(*tempSpeed), std::move(*concSpeed),
			std::move(catalysts));
	}

	const auto& ignored = definition.getRemainingProperties();
	for (const auto& [name, _] : ignored)
		Log(this).warn("Ignored unknown reaction property: '{0}', at: {1}.", name, definition.getLocationName());

	// create
	if (data->mapReactantsToProducts() == false)
	{
		Log(this).error("Malformed reaction with id: {0}, at: {1}.", *id, definition.getLocationName());
		return false;
	}

	maxReactantCount = std::max(static_cast<uint8_t>(reactantIds.size()), maxReactantCount);

	if (table.emplace(*id, std::to_string(*id), std::move(*data)) == false)
		Log(this).warn("Skipped reaction with duplicate id: '{0}', at: {1}.", *id, definition.getLocationName());

	return true;
}

void ReactionRepository::buildNetwork()
{
	// TODO: use fixed data allocation and remove this
	for (size_t i = 0; i < table.size(); ++i)
		network.insert(table[i]);
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
