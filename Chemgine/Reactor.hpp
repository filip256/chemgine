#pragma once

#include <unordered_set>

#include "DataStoreAccessor.hpp"
#include "ConcreteReaction.hpp"
#include "Reactant.hpp"

class Reactor
{
private:
	double stirSpeed = 0.0;
	Amount<Unit::CELSIUS> temperature;
	Amount<Unit::TORR> pressure;

	std::unordered_set<ConcreteReaction, ConcreteReactionHash> cachedReactions;
	std::unordered_set<Reactant, ReactantHash> content;

	static DataStoreAccessor dataAccessor;

	void removeNegligibles();
	void findNewReactions();
	void runReactions();

public:
	Reactor(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure
	) noexcept;
	Reactor(const Reactor&) = delete;
	Reactor(Reactor&&) = default;

	void add(Reactor& other);
	void add(Reactor& other, const double ratio);
	void add(const Reactant& reactant);
	void add(const Molecule& molecule, const Amount<Unit::MOLE> amount);

	Amount<Unit::MOLE> getAmountOf(const Reactant& reactant) const;

	void tick();

	static void setDataStore(const DataStore& dataStore);
};