#include "reactions/kinds/ConcreteReaction.hpp"

ConcreteReaction::ConcreteReaction(const ConcreteReaction& other) noexcept :
    baseReaction(other.baseReaction),
    reactants(other.reactants.makeCopy(NullRef)),
    products(other.products.makeCopy(NullRef))
{}

ConcreteReaction::ConcreteReaction(
    const ReactionData&          baseReaction,
    const std::vector<Reactant>& reactants,
    const std::vector<Molecule>& products) noexcept :
    baseReaction(baseReaction),
    reactants(reactants),
    products(products)
{}

const ReactantSet& ConcreteReaction::getReactants() const { return reactants; }

const ReactantSet& ConcreteReaction::getProducts() const { return products; }

const ImmutableSet<Catalyst>& ConcreteReaction::getCatalysts() const
{
    return baseReaction.getCatalysts();
}

const Amount<Unit::CELSIUS> ConcreteReaction::getReactantTemperature() const
{
    return reactants.any().getLayerTemperature();
}

const ReactionData& ConcreteReaction::getData() const { return baseReaction; }

std::string ConcreteReaction::getHRTag() const { return baseReaction.getHRTag(); }

bool ConcreteReaction::operator==(const ConcreteReaction& other) const
{
    return this->reactants == other.reactants &&
           this->products == other.products &&
           this->baseReaction.getCatalysts() == other.baseReaction.getCatalysts();
}

bool ConcreteReaction::operator!=(const ConcreteReaction& other) const
{
    return this->reactants != other.reactants ||
           this->products != other.products ||
           this->baseReaction.getCatalysts() != other.baseReaction.getCatalysts();
}

ConcreteReaction ConcreteReaction::makeCopy() const { return ConcreteReaction(*this); }
