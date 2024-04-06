#include "DumpContainer.hpp"
#include "Reactant.hpp"
#include "Logger.hpp"

DumpContainer DumpContainer::GlobalDumpContainer = DumpContainer();

void DumpContainer::add(const Reactant& reactant) 
{
	const auto rMass = reactant.getMass();
	if (totalMass.oveflowsOnAdd(rMass))
	{
		Logger::log("DumpContainer: Mass overflowed and was set to 0 (some checks might fail).", LogType::WARN);
		totalMass = 0.0;
		return;
	}

	totalMass += rMass;
}

void DumpContainer::add(const Amount<Unit::JOULE> energy)
{
	if (totalEnergy.oveflowsOnAdd(energy))
	{
		Logger::log("DumpContainer: Energy overflowed and was set to 0 (some checks might fail).", LogType::WARN);
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
