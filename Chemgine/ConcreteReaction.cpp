#include "ConcreteReaction.hpp"

ConcreteReaction::ConcreteReaction(const ConcreteReaction& other) noexcept :
	baseReaction(other.baseReaction),
	reactants(reactants.makeCopy(nullRef)),
	products(reactants.makeCopy(nullRef))
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
	return this->reactants == other.reactants && this->products == other.products;
}

bool ConcreteReaction::operator!=(const ConcreteReaction& other) const
{
	return this->reactants != other.reactants || this->products != other.products;
}

ConcreteReaction ConcreteReaction::makeCopy() const
{
	return ConcreteReaction(*this);
}
