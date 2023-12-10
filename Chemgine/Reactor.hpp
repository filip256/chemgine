#pragma once

#include <unordered_set>
#include <array>

#include "DataStoreAccessor.hpp"
#include "ConcreteReaction.hpp"
#include "Reactant.hpp"
#include "LayerProperties.hpp"

class Reactor
{
private:
	double stirSpeed = 0.0;
	Amount<Unit::CELSIUS> temperature;
	Amount<Unit::TORR> pressure;
	Amount<Unit::MOLE> totalMoles;
	Amount<Unit::LITER> totalVolume;

	const BaseApproximator* temperatureSpeedApproximator = nullptr;
	const BaseApproximator* concentrationSpeedApproximator = nullptr;

	std::array<LayerProperties, REAL_LAYER_COUNT> layerProperties;

	std::unordered_set<ConcreteReaction, ConcreteReactionHash> cachedReactions;
	std::unordered_set<Reactant, ReactantHash> content;

	static DataStoreAccessor dataAccessor;

	void addToLayer(const Reactant& reactant, const uint8_t revert = 1.0);
	void removeFromLayer(const Reactant& reactant);

	void removeNegligibles();
	void findNewReactions();
	void runReactions();
	void distributeUnknownLayer();

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
	Amount<Unit::MOLE> getAmountOf(const ReactantSet& reactantSet) const;

	void tick();

	static void setDataStore(const DataStore& dataStore);
};