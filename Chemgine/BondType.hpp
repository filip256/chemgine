#pragma once

#include <cstdint>

enum BondType : uint8_t
{
    NONE,
    NO_BOND,
    IONIC,
    SINGLE,
    DOUBLE,
    TRIPLE,
    QUADRUPLE,
    AROMATIC,
    LEVO_SINGLE,
    DEXTRO_SINGLE,
};