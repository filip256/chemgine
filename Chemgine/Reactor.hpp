#pragma once

#include <unordered_set>
#include <array>

#include "MultiLayerMixture.hpp"
#include "DataStoreAccessor.hpp"
#include "ConcreteReaction.hpp"
#include "Atmosphere.hpp"
#include "FlagField.hpp"

enum class TickMode : uint8_t
{
	ENABLE_NONE = 0,

	ENABLE_OVERFLOW = 1 << 0,
	ENABLE_NEGLIGIBLES = 1 << 1,
	ENABLE_REACTIONS = 1 << 2,
	ENABLE_CONDUCTION = 1 << 3,
	ENABLE_ENERGY = 1 << 4,

	ENABLE_ALL = static_cast<uint8_t>(-1)
};

class Reactor : public MultiLayerMixture
{
private:
	double stirSpeed = 0.0;

	FlagField<TickMode> tickMode = TickMode::ENABLE_ALL;

	const BaseEstimator* temperatureSpeedEstimator = nullptr;
	const BaseEstimator* concentrationSpeedEstimator = nullptr;

	std::unordered_set<ConcreteReaction> cachedReactions;

	static DataStoreAccessor dataAccessor;

	double getInterLayerReactivityCoefficient(const Reactant& r1, const Reactant& r2) const;
	double getInterLayerReactivityCoefficient(const ReactantSet& reactants) const;

	void findNewReactions();
	void runReactions(const Amount<Unit::SECOND> timespan);
	void runLayerEnergyConduction(const Amount<Unit::SECOND> timespan);
	void consumePotentialEnergy();

	Reactor(const Reactor& other) noexcept;

public:
	Reactor(
		const Ref<Atmosphere> atmosphere,
		const Amount<Unit::LITER> maxVolume,
		const Ref<BaseContainer> overflowTarget
	) noexcept;

	Reactor(
		const Ref<Atmosphere> atmosphere,
		const Amount<Unit::LITER> maxVolume
	) noexcept;

	Reactor(Reactor&&) = default;

	void add(const Molecule& molecule, const Amount<Unit::MOLE> amount);
	void add(const Amount<Unit::JOULE> heat);
	void add(Reactor& other);
	void add(Reactor& other, const double ratio);

	FlagField<TickMode> getTickMode() const;
	void setTickMode(const FlagField<TickMode> mode);
	void tick();

	bool hasSameState(const Reactor& other,
		const Amount<>::StorageType epsilon = std::numeric_limits<Amount<>::StorageType>::epsilon()) const;
	bool hasSameContent(const Reactor& other,
		const Amount<>::StorageType epsilon = std::numeric_limits<Amount<>::StorageType>::epsilon()) const;
	bool hasSameLayers(const Reactor& other,
		const Amount<>::StorageType epsilon = std::numeric_limits<Amount<>::StorageType>::epsilon()) const;
	bool isSame(const Reactor& other,
		const Amount<>::StorageType epsilon = std::numeric_limits<Amount<>::StorageType>::epsilon()) const;

	Reactor makeCopy() const;

	static void setDataStore(const DataStore& dataStore);
};