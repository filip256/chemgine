#pragma once

#include "BondType.hpp"
#include "Atom.hpp"

class Bond
{
private:
    BondType type;

public:
    c_size other;

    Bond(const c_size other, const BondType type);
    Bond(const Bond&) = default;

    BondType getType() const;
    uint8_t getValence() const;
    std::string getSMILES() const;

    static std::string getSMILES(const BondType type);
    static BondType fromSMILES(const char symbol);
    static uint8_t getValence(const BondType type);
};
