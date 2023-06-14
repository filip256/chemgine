#pragma once

#include "BondType.hpp"
#include "Atom.hpp"

class Bond
{
    const BondType type;
    const Atom& other;

public:
    Bond(const Atom& other, const BondType type);
};

