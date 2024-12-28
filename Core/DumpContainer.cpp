#include "DumpContainer.hpp"
#include "Reactant.hpp"
#include "Log.hpp"

DumpContainer DumpContainer::GlobalDumpContainer = DumpContainer();

void DumpContainer::add(const Reactant& reactant) 
{
	const auto rMass = reactant.getMass().asStd() * _Gram;
	if (totalMass.oveflowsOnAdd(rMass))
	{
		Log(this).warn("Mass overflowed and was set to 0 (some checks might fail).");
		totalMass = 0.0f * _Gram;
		return;
	}

	totalMass += rMass;
}

void DumpContainer::add(const Quantity<Joule> energy)
{
	if (totalEnergy.oveflowsOnAdd(energy))
	{
		Log(this).warn("Energy overflowed and was set to 0 (some checks might fail).");
		totalEnergy = 0.0f * _Joule;
		return;
	}

	totalEnergy += energy;
}


Quantity<Gram> DumpContainer::getTotalMass() const
{
	return totalMass;
}

Quantity<Joule> DumpContainer::getTotalEnergy() const
{
	return totalEnergy;
}
