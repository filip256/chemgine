#pragma once

#include "BondType.hpp"
#include "Atom.hpp"

class Bond
{
public:
    const BondType type;
    const size_t other;

    Bond(const size_t other, const BondType type);

    static BondType fromSMILES(const char symbol);
};

