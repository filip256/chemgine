#pragma once

#include "ContainerBase.hpp"

/// <summary>
/// Reactant container with no storage or properties. Just a dump for reactants.
/// </summary>
class DumpContainer final : public ContainerBase
{
private:
	Quantity<Gram> totalMass = 0.0f * _Gram;
	Quantity<Joule> totalEnergy = 0.0f * _Joule;

public:
	void add(const Reactant& reactant) override final;
	void add(const Quantity<Joule> energy) override final;

	Quantity<Gram> getTotalMass() const;
	Quantity<Joule> getTotalEnergy() const;

	static DumpContainer GlobalDumpContainer;
};
