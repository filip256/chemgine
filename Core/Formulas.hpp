#pragma once

#include "Units.hpp"

class Formulas
{
public:
	static Quantity<Liter> idealGasLaw(
		const Quantity<AbsKelvin> temperature,
		const Quantity<Atmosphere> pressure,
		const Quantity<Mole> moles
	);

	static Quantity<GramPerMilliLiter> idealGasLaw(
		const Quantity<AbsKelvin> temperature,
		const Quantity<Atmosphere> pressure,
		const Quantity<GramPerMole> molarMass
	);

	static Quantity<JoulePerMoleCelsius> isobaricHeatCapacity(const uint8_t degreesOfFreedom);

	static Quantity<JoulePerMoleCelsius> isochoricHeatCapacity(const uint8_t degreesOfFreedom);
};
