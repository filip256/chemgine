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
    std::string toSMILES() const;

    static std::string toSMILES(const BondType type);
    static BondType fromSMILES(const char symbol);
    static uint8_t getValence(const BondType type);


    // for memory leak checking 
    static size_t instanceCount;
#ifndef NDEBUG
    void* operator new(const size_t count);
    void operator delete(void* ptr);
#endif
};

