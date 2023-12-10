#pragma once

#include "Amount.hpp"

class Reactor;

class LayerProperties
{
private:
	Amount<Unit::LITER> volume;
	Amount<Unit::MOLE> moles;

public:
	LayerProperties() = default;

	friend class Reactor;
};