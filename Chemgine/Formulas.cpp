#include "Formulas.hpp"
#include "Constants.hpp"

Amount<Unit::GRAM_PER_MILLILITER> Formulas::idealGasLaw(
	const Amount<Unit::KELVIN> temperature,
	const Amount<Unit::PASCAL> pressure,
	const Amount<Unit::GRAM_PER_MOLE> molarMass
)
{
	return Amount<Unit::GRAM_PER_MILLILITER>(
		molarMass.asStd() / ((temperature.asStd() / pressure.asStd()) * Constants::IDEAL_GAS_CONSTANT * 1000000)
	);
}

Amount<Unit::JOULE_PER_MOLE> Formulas::isobaricHeatCapacity(const uint8_t degreesOfFreedom)
{
	return Amount<Unit::JOULE_PER_MOLE>((degreesOfFreedom + 2) * Constants::IDEAL_GAS_CONSTANT / 2.0);
}

Amount<Unit::JOULE_PER_MOLE> Formulas::isochoricHeatCapacity(const uint8_t degreesOfFreedom)
{
	return Amount<Unit::JOULE_PER_MOLE>(degreesOfFreedom * Constants::IDEAL_GAS_CONSTANT / 2.0);
}