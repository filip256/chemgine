#pragma once

#include "Atom.hpp"
#include "RadicalData.hpp"

class Radical : public Atom
{
public:
    Radical(const Symbol& symbol) noexcept;

    const RadicalData& getData() const override final;

    bool matches(const Atom& other) const override final;

    std::unique_ptr<Atom> clone() const override final;

    static bool isDefined(const Symbol& symbol);
};
