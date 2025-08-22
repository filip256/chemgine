#pragma once

#include "ContainerBase.hpp"
#include "data/values/Amount.hpp"

/// <summary>
/// Reactant container with no storage or properties. Just a dump for reactants.
/// </summary>
class DumpContainer final : public ContainerBase
{
private:
    Amount<Unit::GRAM>  totalMass   = 0.0;
    Amount<Unit::JOULE> totalEnergy = 0.0;

public:
    void add(const Reactant& reactant) override final;
    void addEnergy(const Amount<Unit::JOULE> energy) override final;

    Amount<Unit::GRAM>  getTotalMass() const;
    Amount<Unit::JOULE> getTotalEnergy() const;

    static DumpContainer GlobalDumpContainer;
};
