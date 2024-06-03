#pragma once

#include "Atom.hpp"
#include "RadicalData.hpp"

class Radical : public Atom
{
public:
    Radical(const AtomId id) noexcept;
    Radical(const Symbol symbol) noexcept;

    const RadicalData& data() const override final;

    bool matches(const Atom& other) const override final;

    Radical* clone() const override final;

    static bool isDefined(const AtomId id);
    static bool isDefined(const Symbol symbol);
};
