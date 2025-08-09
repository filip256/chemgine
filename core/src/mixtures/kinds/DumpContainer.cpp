#include "mixtures/kinds/DumpContainer.hpp"

#include "reactions/Reactant.hpp"
#include "io/Log.hpp"

DumpContainer DumpContainer::GlobalDumpContainer = DumpContainer();

void DumpContainer::add(const Reactant& reactant) 
{
	const auto rMass = reactant.getMass();
	if (totalMass.oveflowsOnAdd(rMass))
	{
		Log(this).warn("Mass overflowed and was set to 0 (some checks might fail).");
		totalMass = 0.0;
		return;
	}

	totalMass += rMass;
}

void DumpContainer::add(const Amount<Unit::JOULE> energy)
{
	if (totalEnergy.oveflowsOnAdd(energy))
	{
		Log(this).warn("Energy overflowed and was set to 0 (some checks might fail).");
		totalMass = 0.0;
		return;
	}

	totalEnergy += energy;
}


Amount<Unit::GRAM> DumpContainer::getTotalMass() const
{
	return totalMass;
}

Amount<Unit::JOULE> DumpContainer::getTotalEnergy() const
{
	return totalEnergy;
}
