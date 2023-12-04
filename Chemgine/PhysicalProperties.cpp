#include "PhysicalProperties.hpp"

PhysicalProperties::PhysicalProperties(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) noexcept:
	temperature(temperature),
	pressure(pressure)
{}