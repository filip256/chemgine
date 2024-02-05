#pragma once

#include "ReactionData.hpp"
#include "ReactantSet.hpp"
#include "HashCombine.hpp"

#include <vector>

class ConcreteReaction
{
private:
	const ReactionData& baseReaction;
	ReactantSet reactants;
	ReactantSet products;

	ConcreteReaction(const ConcreteReaction& other) noexcept;

public:
	ConcreteReaction(
		const ReactionData& baseReaction,
		const std::vector<Reactant>& reactants,
		const std::vector<Molecule>& products
	) noexcept;

	ConcreteReaction(ConcreteReaction&&) = default;

	bool isEquivalent(const ConcreteReaction& other) const;

	const ReactantSet& getReactants() const;
	const ReactantSet& getProducts() const;

	const Amount<Unit::CELSIUS> getReactantTemperature() const;

	const ReactionData& getData() const;

	bool operator==(const ConcreteReaction& other) const;
	bool operator!=(const ConcreteReaction& other) const;

	ConcreteReaction makeCopy() const;

	friend struct std::hash<ConcreteReaction>;
};


template<>
struct std::hash<ConcreteReaction>
{
	size_t operator() (const ConcreteReaction& reaction) const
	{
		size_t hash = 0;
		for (const auto& r : reaction.reactants)
			hashCombineWith(hash, r.molecule.getId());
		for (const auto& p : reaction.products)
			hashCombineWith(hash, p.molecule.getId());
		return hash;
	}
};