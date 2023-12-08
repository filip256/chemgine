#pragma once

#include "Amount.hpp"

class PhysicalProperties
{
public:
	Amount<Unit::CELSIUS> temperature;
	Amount<Unit::TORR> pressure;

	PhysicalProperties(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure
	) noexcept;

	PhysicalProperties(const PhysicalProperties&) = default;
};