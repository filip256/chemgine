#pragma once

#include "BondType.hpp"
#include "utils/ASCII.hpp"

#include <string>

class BondedAtomBase;

class Bond
{
private:
    BondType        type;
    BondedAtomBase* other;

public:
    Bond(BondedAtomBase& other, const BondType type);
    Bond(const Bond&) = default;

    const BondedAtomBase& getOther() const;
    BondedAtomBase&       getOther();

    void setOther(BondedAtomBase& other);

    BondType    getType() const;
    uint8_t     getValence() const;
    std::string getSMILES() const;
    char        getASCII(const ASCII::Direction direction) const;
    float_s     getASCIIScore(const ASCII::Direction direction) const;

    static std::string getSMILES(const BondType type);
    static BondType    fromSMILES(const char symbol);

    static char     getASCII(const BondType type, const ASCII::Direction direction);
    static BondType fromASCII(const char symbol);
    static bool     isInDirection(const char symbol, const ASCII::Direction direction);
    static bool     hasCompleteASCIIRepresentation(const BondType type);
    static float_s  getASCIIScore(const BondType type, const ASCII::Direction direction);

    static uint8_t getValence(const BondType type);
};
