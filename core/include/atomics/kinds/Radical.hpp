#pragma once

#include "atomics/data/RadicalData.hpp"
#include "atomics/kinds/AtomBase.hpp"

class Radical : public Cloneable<Radical, AtomBase>
{
private:
    using Base = Cloneable<Radical, AtomBase>;
    using Base::isDefined;

public:
    using Base::Cloneable;

    const RadicalData& getData() const override final;

    bool matches(const AtomBase& other) const override final;

    static std::optional<Radical> fromSymbol(const Symbol& symbol);
};
