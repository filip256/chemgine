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
	Amount<Unit::CELSIUS> temperature;

	Amount<Unit::MOLE> moles;
	Amount<Unit::GRAM> mass;
	Amount<Unit::LITER> volume;
	Amount<Unit::JOULE> potentialEnergy;

public:
	LayerProperties(const Amount<Unit::CELSIUS> temperature = 0.0) noexcept;

	Amount<Unit::CELSIUS> getTemperature() const;
	Amount<Unit::MOLE> getMoles() const;
	Amount<Unit::GRAM> getMass() const;
	Amount<Unit::LITER> getVolume() const;

	bool isEmpty() const;

	template <LayerType L>
	friend class SingleLayerMixture;
	friend class MultiLayerMixture;
	friend class Reactor;
};