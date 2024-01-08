#pragma once

#include "ReactionData.hpp"
#include "ReactantSet.hpp"

#include <vector>

class ConcreteReaction
{
private:
	const ReactionData& baseReaction;
	ReactantSet reactants;
	ReactantSet products;

public:
	ConcreteReaction(
		const ReactionData& baseReaction,
		const std::vector<Reactant>& reactants,
		const std::vector<Molecule>& products
	) noexcept;

	bool isEquivalent(const ConcreteReaction& other) const;

	const ReactantSet& getReactants() const;
	const ReactantSet& getProducts() const;

	const ReactionData& getData() const;

	bool operator==(const ConcreteReaction& other) const;
	bool operator!=(const ConcreteReaction& other) const;

	friend class ConcreteReactionHash;
};

class ConcreteReactionHash
{
public:
	size_t operator() (const ConcreteReaction& reaction) const;
};