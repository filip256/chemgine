#include "Formulas.hpp"
#include "Constants.hpp"

Quantity<Liter> Formulas::idealGasLaw(
	const Quantity<AbsKelvin> temperature,
	const Quantity<Atmosphere> pressure,
	const Quantity<Mole> moles)
{
	return ((moles * temperature) / pressure).value() * Constants::IDEAL_GAS_CONSTANT * _Liter / 100.0;
}

Quantity<GramPerMilliLiter> Formulas::idealGasLaw(
	const Quantity<AbsKelvin> temperature,
	const Quantity<Atmosphere> pressure,
	const Quantity<GramPerMole> molarMass)
{
	return ((molarMass * _Mole) / idealGasLaw(temperature, pressure, _Mole))
		.to<GramPerMilliLiter>();
}

Quantity<JoulePerMoleCelsius> Formulas::isobaricHeatCapacity(const uint8_t degreesOfFreedom)
{
	return Quantity<JoulePerMoleCelsius>::from((degreesOfFreedom + 2) * Constants::IDEAL_GAS_CONSTANT / 2.0);
}

Quantity<JoulePerMoleCelsius> Formulas::isochoricHeatCapacity(const uint8_t degreesOfFreedom)
{
	return Quantity<JoulePerMoleCelsius>::from(degreesOfFreedom * Constants::IDEAL_GAS_CONSTANT / 2.0);
}
