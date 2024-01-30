#include "Mixture.hpp"

void Mixture::add(const Reactant& reactant)
{
	content.add(reactant);
}

Amount<Unit::MOLE> Mixture::getAmountOf(const Reactant& reactant) const
{
	return content.getAmountOf(reactant);
}

Amount<Unit::MOLE> Mixture::getAmountOf(const ReactantSet& reactantSet) const
{
	return content.getAmountOf(reactantSet);
}