#include "Mixture.hpp"

Mixture::Mixture(const Mixture& other) noexcept:
	content(other.content.makeCopy())
{}

void Mixture::add(const Reactant& reactant)
{
	content.add(reactant);
}

void Mixture::add(const Mixture& other)
{
	for (const auto& r : other.content)
		add(r);
}

Amount<Unit::MOLE> Mixture::getAmountOf(const Reactant& reactant) const
{
	return content.getAmountOf(reactant);
}

Amount<Unit::MOLE> Mixture::getAmountOf(const ReactantSet& reactantSet) const
{
	return content.getAmountOf(reactantSet);
}