#pragma once

#include "Amount.hpp"
#include "LayerType.hpp"
#include "StateNucleator.hpp"
#include "Ref.hpp"
#include "LayerIterator.hpp"

class Mixture;
template <LayerType L>
class SingleLayerMixture;
class MultiLayerMixture;
class Reactor;

class Layer
{
private:
	const LayerType layerType;

	Amount<Unit::CELSIUS> temperature = 0.0;

	Amount<Unit::MOLE> moles = 0.0;
	Amount<Unit::GRAM> mass = 0.0;
	Amount<Unit::LITER> volume = 0.0;
	Amount<Unit::JOULE> potentialEnergy = 0.0;

	Polarity polarity = Polarity(0.0, 0.0);

	Ref<Mixture> container;

	StateNucleator lowNucleator;
	StateNucleator highNucleator;

	void findNewLowNucleator();
	void findNewHighNucleator();

	void consumePositivePotentialEnergy();
	void consumeNegativePotentialEnergy();

	// Returns the least amount of energy required to react a target temperature.
	// More energy might be needed.
	Amount<Unit::JOULE> getLeastEnergyDiff(const Amount<Unit::CELSIUS> target) const;

	bool hasTemporaryState(const Reactant& reactant) const;
	void convertTemporaryStateReactants();

public:
	Layer(
		const Ref<Mixture> container = Ref<Mixture>::nullRef,
		const LayerType layerType = LayerType::NONE,
		const Amount<Unit::CELSIUS> temperature = 0.0
	) noexcept;

	Amount<Unit::MOLE> getMoles() const;
	Amount<Unit::GRAM> getMass() const;
	Amount<Unit::LITER> getVolume() const;
	Amount<Unit::CELSIUS> getTemperature() const;
	Amount<Unit::CELSIUS> getMinAllowedTemperature() const;
	Amount<Unit::CELSIUS> getMaxAllowedTemperature() const;
	Amount<Unit::JOULE_PER_MOLE_CELSIUS> getHeatCapacity() const;
	Amount<Unit::JOULE_PER_CELSIUS> getTotalHeatCapacity() const;
	Amount<Unit::JOULE_PER_MOLE> getKineticEnergy() const;
	Polarity getPolarity() const;

	bool isEmpty() const;

	bool hasLowNucleator() const;
	bool hasHighNucleator() const;
	const Reactant& getLowNucleator() const;
	const Reactant& getHighNucleator() const;

	void setIfNucleator(const Reactant& reactant);
	void unsetIfNucleator(const Reactant& reactant);

	void consumePotentialEnergy();

	bool equals(const Layer& other,
		const Amount<>::StorageType epsilon = std::numeric_limits<Amount<>::StorageType>::epsilon()) const;

	LayerIterator begin() const;
	LayerIterator end() const;

	template <LayerType L>
	friend class SingleLayerMixture;
	friend class MultiLayerMixture;
	friend class Reactor;
};