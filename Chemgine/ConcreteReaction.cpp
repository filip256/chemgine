#include "ConcreteReaction.hpp"
#include "HashCombine.hpp"

ConcreteReaction::ConcreteReaction(
	const ReactionData& baseReaction,
	const std::vector<Reactant>& reactants,
	const std::vector<Molecule>& products
) noexcept :
	baseReaction(baseReaction),
	reactants(reactants),
	products(products)
{}

bool ConcreteReaction::isEquivalent(const ConcreteReaction& other) const
{
	for (const auto& r : this->reactants)
		if (other.reactants.contains(r) == false)
			return false;

	for (const auto& r : this->products)
		if (other.products.contains(r) == false)
			return false;

	return true;
}

const ReactantSet& ConcreteReaction::getReactants() const
{
	return reactants;
}

const ReactantSet& ConcreteReaction::getProducts() const
{
	return products;
}

const ReactionData& ConcreteReaction::getData() const
{
	return baseReaction;
}



bool ConcreteReaction::operator==(const ConcreteReaction& other) const
{
	return isEquivalent(other);
}

bool ConcreteReaction::operator!=(const ConcreteReaction& other) const
{
	return !isEquivalent(other);
}




size_t ConcreteReactionHash::operator() (const ConcreteReaction& reaction) const
{
	size_t hash = 0;
	for (const auto& r : reaction.reactants)
		hashCombine(hash, r.molecule.getId());
	for (const auto& p : reaction.products)
		hashCombine(hash, p.molecule.getId());
	return hash;
}