#pragma once

#include "data/values/Amount.hpp"

class Formulas
{
public:
    static Amount<Unit::LITER> idealGasLaw(
        const Amount<Unit::KELVIN> temperature, const Amount<Unit::ATMOSPHERE> pressure, const Amount<Unit::MOLE> moles);

    static Amount<Unit::GRAM_PER_MILLILITER> idealGasLaw(
        const Amount<Unit::KELVIN>        temperature,
        const Amount<Unit::ATMOSPHERE>    pressure,
        const Amount<Unit::GRAM_PER_MOLE> molarMass);

    static Amount<Unit::JOULE_PER_MOLE_CELSIUS> isobaricHeatCapacity(const uint8_t degreesOfFreedom);

    static Amount<Unit::JOULE_PER_MOLE_CELSIUS> isochoricHeatCapacity(const uint8_t degreesOfFreedom);
};
