#include "data/Formulas.hpp"

#include "data/values/Constants.hpp"

Amount<Unit::LITER> Formulas::idealGasLaw(
    const Amount<Unit::KELVIN>     temperature,
    const Amount<Unit::ATMOSPHERE> pressure,
    const Amount<Unit::MOLE>       moles)
{
    return ((moles.asStd() * temperature.asStd()) / pressure.asStd()) *
           Constants::IDEAL_GAS_CONSTANT /
           100.0f;
}

Amount<Unit::GRAM_PER_MILLILITER> Formulas::idealGasLaw(
    const Amount<Unit::KELVIN>        temperature,
    const Amount<Unit::ATMOSPHERE>    pressure,
    const Amount<Unit::GRAM_PER_MOLE> molarMass)
{
    return idealGasLaw(temperature, pressure, 1.0_mol)
        .to<Unit::GRAM_PER_MILLILITER>(molarMass.to<Unit::GRAM>(1.0_mol));
}

Amount<Unit::JOULE_PER_MOLE_CELSIUS> Formulas::isobaricHeatCapacity(const uint8_t degreesOfFreedom)
{
    return Amount<Unit::JOULE_PER_MOLE_CELSIUS>(
        (degreesOfFreedom + 2) * Constants::IDEAL_GAS_CONSTANT / 2.0f);
}

Amount<Unit::JOULE_PER_MOLE_CELSIUS> Formulas::isochoricHeatCapacity(const uint8_t degreesOfFreedom)
{
    return Amount<Unit::JOULE_PER_MOLE_CELSIUS>(
        degreesOfFreedom * Constants::IDEAL_GAS_CONSTANT / 2.0f);
}
