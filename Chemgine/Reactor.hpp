#pragma once

#include <unordered_set>
#include <array>

#include "MultiLayerMixture.hpp"
#include "DataStoreAccessor.hpp"
#include "ConcreteReaction.hpp"

class Reactor : public MultiLayerMixture
{
private:
	double stirSpeed = 0.0;

	const BaseEstimator* temperatureSpeedEstimator = nullptr;
	const BaseEstimator* concentrationSpeedEstimator = nullptr;

	std::unordered_set<ConcreteReaction> cachedReactions;

	static DataStoreAccessor dataAccessor;

	void findNewReactions();
	void runReactions(const Amount<Unit::SECOND> timespan);
	void consumePotentialEnergy();

public:
	Reactor(
		SingleLayerMixture<LayerType::GASEOUS>& atmosphere,
		const Amount<Unit::LITER> maxVolume,
		Mixture* overflowTarget
	) noexcept;

	Reactor(
		SingleLayerMixture<LayerType::GASEOUS>& atmosphere,
		const Amount<Unit::LITER> maxVolume
	) noexcept;

	Reactor(const Reactor&) = delete;
	Reactor(Reactor&&) = default;

	void add(const Molecule& molecule, const Amount<Unit::MOLE> amount);
	void add(const Amount<Unit::JOULE> heat);
	void add(Reactor& other);
	void add(Reactor& other, const double ratio);

	void tick();

	static void setDataStore(const DataStore& dataStore);

	friend class Reactant;
};