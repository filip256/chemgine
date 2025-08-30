#pragma once

#include <cstdint>

enum BondType : uint8_t
{
    NONE,
    NON_BOND,
    IONIC,
    SINGLE,
    DOUBLE,
    TRIPLE,
    QUADRUPLE,
    AROMATIC,
    LEVO_SINGLE,
    DEXTRO_SINGLE,

    BOND_TYPE_COUNT
};
