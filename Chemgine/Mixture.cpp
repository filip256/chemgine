#include "Mixture.hpp"

Mixture::Mixture(const Mixture& other) noexcept :
	content(other.content.makeCopy(*this))
{}

void Mixture::add(const Reactant& reactant)
{
	content.add(reactant);
}

void Mixture::add(const Mixture& other)
{
	for (const auto& [_, r] : other.content)
		add(r);
}

const ReactantSet& Mixture::getContent() const
{
	return content;
}

Amount<Unit::MOLE> Mixture::getAmountOf(const Reactant& reactant) const
{
	return content.getAmountOf(reactant);
}

Amount<Unit::MOLE> Mixture::getAmountOf(const ReactantSet& reactantSet) const
{
	return content.getAmountOf(reactantSet);
}