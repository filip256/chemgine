#pragma once

#include <cstdint>

enum class Unit : uint16_t
{
    NONE,
    ANY,

    LITER,
    CUBIC_METER,
    DROP,

    GRAM,
    MOLE,

    SECOND,
    PER_SECOND,

    CELSIUS,
    KELVIN,
    FAHRENHEIT,

    TORR,
    PASCAL,
    ATMOSPHERE,

    JOULE,
    WATT,

    METER,
    PER_METER,

    DEGREE,
    RADIAN,

    MOLE_RATIO,
    MOLE_PERCENT,

    MOLE_PER_SECOND,
    GRAM_PER_MOLE,
    GRAM_PER_MILLILITER,
    JOULE_PER_MOLE,
    JOULE_PER_CELSIUS,
    JOULE_PER_MOLE_CELSIUS,
    TORR_MOLE_RATIO
};
