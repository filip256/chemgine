#pragma once

#include "Amount.hpp"

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
	LayerProperties(Amount<Unit::CELSIUS> temperature = 0.0) noexcept;

	Amount<Unit::CELSIUS> getTemperature() const;
	Amount<Unit::MOLE> getMoles() const;
	Amount<Unit::GRAM> getMass() const;
	Amount<Unit::LITER> getVolume() const;

	bool isEmpty() const;

	friend class Reactor;
};