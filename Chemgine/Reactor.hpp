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
	Amount<Unit::TORR> pressure;
	Amount<Unit::MOLE> totalMoles;
	Amount<Unit::GRAM> totalMass;
	Amount<Unit::LITER> totalVolume;

	const BaseEstimator* temperatureSpeedEstimator = nullptr;
	const BaseEstimator* concentrationSpeedEstimator = nullptr;

	std::unordered_set<ConcreteReaction, ConcreteReactionHash> cachedReactions;
	std::unordered_set<Reactant, ReactantHash> content;

	std::unordered_map<LayerType, LayerProperties> layers;

	static DataStoreAccessor dataAccessor;

	bool tryCreateLayer(const LayerType layer);
	void addToLayer(const Reactant& reactant, const uint8_t revert = 1.0);
	void removeFromLayer(const Reactant& reactant);

	void removeNegligibles();
	void findNewReactions();
	void runReactions(const Amount<Unit::SECOND> timespan);
	void consumePotentialEnergy();

	LayerType getLayerAbove(LayerType layer) const;
	LayerType getLayerBelow(LayerType layer) const;

	void add(const Amount<Unit::JOULE> heat, const LayerType layer);

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
	void add(const Amount<Unit::JOULE> heat);

	LayerType findLayerFor(const Reactant& reactant) const;

	Amount<Unit::JOULE_PER_MOLE_CELSIUS> getLayerHeatCapacity(const LayerType layer) const;
	Amount<Unit::JOULE_PER_MOLE> getLayerKineticEnergy(const LayerType layer) const;

	Amount<Unit::MOLE> getAmountOf(const Reactant& reactant) const;
	Amount<Unit::MOLE> getAmountOf(const ReactantSet& reactantSet) const;

	Amount<Unit::TORR> getPressure() const;
	Amount<Unit::MOLE> getTotalMoles() const;
	Amount<Unit::GRAM> getTotalMass() const;
	Amount<Unit::LITER> getTotalVolume() const;

	bool hasLayer(const LayerType layer) const;

	void tick();

	static void setDataStore(const DataStore& dataStore);

	friend class Reactant;
};