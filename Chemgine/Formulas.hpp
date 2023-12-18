#pragma once

#include "Amount.hpp"

class Formulas
{
public:
	static Amount<Unit::GRAM_PER_MILLILITER> idealGasLaw(
		const Amount<Unit::KELVIN> temperature,
		const Amount<Unit::PASCAL> pressure,
		const Amount<Unit::GRAM_PER_MOLE> molarMass
	);
};