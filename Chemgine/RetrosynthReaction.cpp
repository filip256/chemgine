#include "RetrosynthReaction.hpp"
#include "Utils.hpp"

RetrosynthReaction::RetrosynthReaction(
	const ReactionData& baseReaction,
	const std::vector<Reactable>& reactants,
	const std::vector<Reactable>& products
) noexcept :
	baseReaction(baseReaction),
	reactants(Utils::aggregate<Reactable, uint8_t>(reactants)),
	products(Utils::aggregate<Reactable, uint8_t>(products))
{}

const std::unordered_map<Reactable, uint8_t>& RetrosynthReaction::getReactants() const
{
	return reactants;
}

const std::unordered_map<Reactable, uint8_t>& RetrosynthReaction::getProducts() const
{
	return products;
}

const ReactionData& RetrosynthReaction::getBaseData() const
{
	return baseReaction;
}

bool RetrosynthReaction::operator==(const RetrosynthReaction& other) const
{
	return &this->baseReaction == &other.baseReaction &&
		this->reactants == other.reactants;
}

bool RetrosynthReaction::operator!=(const RetrosynthReaction& other) const
{
	return &this->baseReaction != &other.baseReaction ||
		this->reactants != other.reactants;
}
