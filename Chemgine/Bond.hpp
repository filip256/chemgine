#pragma once

#include "BondType.hpp"
#include "Atom.hpp"

class Bond
{
public:
    const BondType type;
    const size_t other;

    Bond(const size_t other, const BondType type);

    uint8_t getValence() const;

    static BondType fromSMILES(const char symbol);
    static uint8_t getValence(const BondType type);
};

