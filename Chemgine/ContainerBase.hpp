#pragma once

#include "Amount.hpp"

class Reactant;

/// <summary>
/// Abstract base for reactant containers.
/// </summary>
class ContainerBase
{
public:
	virtual ~ContainerBase() = default;

	virtual void add(const Reactant& reactant) = 0;
	virtual void add(const Amount<Unit::JOULE> energy) = 0;
};
