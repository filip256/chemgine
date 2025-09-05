#pragma once

#include "data/values/Amount.hpp"

class STP
{
public:
    static inline const Amount<Unit::CELSIUS> temperature = 0.0;
    static inline const Amount<Unit::TORR>    pressure    = 760.0;
};
