#pragma once

#include "atomics/data/AtomData.hpp"
#include "atomics/kinds/AtomBase.hpp"

class Atom : public Cloneable<Atom, AtomBase>
{
private:
    using Base = Cloneable<Atom, AtomBase>;
    using Base::isDefined;

public:
    using Base::Cloneable;

    const AtomData& getData() const override final;

    bool matches(const AtomBase& other) const override final;

    static std::optional<Atom> fromSymbol(const Symbol& symbol);
};
