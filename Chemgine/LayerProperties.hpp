#pragma once

#include "Amount.hpp"
#include "LayerType.hpp"

template <LayerType L>
class SingleLayerMixture;
class MultiLayerMixture;
class Reactor;

class LayerProperties
{
private:
	Amount<Unit::CELSIUS> temperature = 0.0;

	Amount<Unit::MOLE> moles = 0.0;
	Amount<Unit::GRAM> mass = 0.0;
	Amount<Unit::LITER> volume = 0.0;
	Amount<Unit::JOULE> potentialEnergy = 0.0;

public:
	LayerProperties(const Amount<Unit::CELSIUS> temperature = 0.0) noexcept;

	Amount<Unit::CELSIUS> getTemperature() const;
	Amount<Unit::MOLE> getMoles() const;
	Amount<Unit::GRAM> getMass() const;
	Amount<Unit::LITER> getVolume() const;

	bool isEmpty() const;

	bool operator==(const LayerProperties& other) const;
	bool operator!=(const LayerProperties& other) const;

	template <LayerType L>
	friend class SingleLayerMixture;
	friend class MultiLayerMixture;
	friend class Reactor;
};