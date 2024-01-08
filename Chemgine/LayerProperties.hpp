#pragma once

#include "Amount.hpp"

class Reactor;

class LayerProperties
{
private:
	Amount<Unit::MOLE> moles;
	Amount<Unit::GRAM> mass;
	Amount<Unit::LITER> volume;

public:
	LayerProperties() = default;

	friend class Reactor;
};