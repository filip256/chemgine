#pragma once

#include "data/values/Amount.hpp"

class STP
{
public:
	inline static const Amount<Unit::CELSIUS> temperature = 0.0;
	inline static const Amount<Unit::TORR> pressure = 760.0;
};
