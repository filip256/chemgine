#pragma once

#include "BondType.hpp"

#include <string>

class BondedAtomBase;

class Bond
{
private:
    BondType type;
    BondedAtomBase* other;

public:
    Bond(BondedAtomBase& other, const BondType type);
    Bond(const Bond&) = default;

    const BondedAtomBase& getOther() const;
    BondedAtomBase& getOther();

    void setOther(BondedAtomBase& other);

    BondType getType() const;
    uint8_t getValence() const;
    std::string getSMILES() const;

    static std::string getSMILES(const BondType type);
    static BondType fromSMILES(const char symbol);

    static uint8_t getValence(const BondType type);
};
