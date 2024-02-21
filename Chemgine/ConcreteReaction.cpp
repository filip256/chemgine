#include "ConcreteReaction.hpp"

ConcreteReaction::ConcreteReaction(const ConcreteReaction& other) noexcept :
	baseReaction(other.baseReaction),
	reactants(reactants.makeCopy(Ref<Mixture>::nullRef)),
	products(reactants.makeCopy(Ref<Mixture>::nullRef))
{}

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

const Amount<Unit::CELSIUS> ConcreteReaction::getReactantTemperature() const
{
	return reactants.any().getLayerTemperature();
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

ConcreteReaction ConcreteReaction::makeCopy() const
{
	return ConcreteReaction(*this);
}