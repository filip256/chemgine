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