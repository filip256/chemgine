#pragma once

#include "AtomData.hpp"
#include "Accessor.hpp"
#include "SizeTypedefs.hpp"

#include <unordered_map>

class Atom : public Accessor<>
{
protected:
    const AtomData& data;

public:
    Atom(const Symbol& symbol) noexcept;
    Atom(const Atom&) = default;
    virtual ~Atom() = default;

    virtual const AtomData& getData() const;

    bool isRadical() const;

    uint8_t getPrecedence() const;
    std::string getSymbol() const;
    std::string getSMILES() const;
    std::unordered_map<Symbol, c_size> getComponentCountMap() const;

    bool equals(const Atom& other) const;
    virtual bool matches(const Atom& other) const;

    bool operator==(const Atom& other) const;
    bool operator!=(const Atom& other) const;

    virtual std::unique_ptr<Atom> clone() const;

    static bool isDefined(const Symbol& symbol);
};
