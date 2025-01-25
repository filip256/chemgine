#pragma once

#include "BondType.hpp"

#include <string>

class BondedAtom;

class Bond
{
private:
    BondType type;
    const BondedAtom* other;

public:
    Bond(const BondedAtom& other, const BondType type);
    Bond(const Bond&) = default;

    BondType getType() const;
    const BondedAtom& getOther() const;
    uint8_t getValence() const;
    std::string getSMILES() const;

    void setOther(const BondedAtom& other);

    static std::string getSMILES(const BondType type);
    static BondType fromSMILES(const char symbol);

    static uint8_t getValence(const BondType type);
};
