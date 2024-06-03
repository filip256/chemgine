#pragma once

#include "AtomData.hpp"
#include "Accessor.hpp"
#include "SizeTypedefs.hpp"

#include <unordered_map>

class Atom : public Accessor<AtomData>
{
public:
    const AtomId id;

    Atom(const AtomId id) noexcept;
    Atom(const Symbol symbol) noexcept;
    Atom(const Atom&) = default;

    virtual const AtomData& data() const;

    bool isRadical() const;

    uint8_t getPrecedence() const;
    std::string getSMILES() const;
    std::unordered_map<AtomId, c_size> getComponentCountMap() const;

    bool equals(const Atom& other) const;
    virtual bool matches(const Atom& other) const;

    bool operator==(const Atom& other) const;
    bool operator!=(const Atom& other) const;

    virtual Atom* clone() const;

    static bool isDefined(const AtomId id);
    static bool isDefined(const Symbol symbol);


    // for memory leak checking 
    static size_t instanceCount;
#ifndef NDEBUG
    void* operator new(const size_t count);
    void operator delete(void* ptr);
#endif
};
