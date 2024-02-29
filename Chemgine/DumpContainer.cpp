#include "DumpContainer.hpp"
#include "Reactant.hpp"

DumpContainer DumpContainer::globalDumpContainer = DumpContainer();

void DumpContainer::add(const Reactant& reactant) 
{
	totalMass += reactant.getMass();
}

Amount<Unit::GRAM> DumpContainer::getTotalMass() const
{
	return totalMass;
}