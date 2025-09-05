#pragma once

#include "data/values/Amount.hpp"

class Reactant;

/// <summary>
/// Abstract base for reactant containers.
/// </summary>
class ContainerBase
{
public:
    virtual ~ContainerBase() = default;

    virtual void add(const Reactant& reactant)               = 0;
    virtual void addEnergy(const Amount<Unit::JOULE> energy) = 0;
};
