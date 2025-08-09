#include "mixtures/kinds/Mixture.hpp"
#include "reactions/Catalyst.hpp"

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

void Mixture::add(const Molecule& molecule, const Amount<Unit::MOLE> amount)
{
	add(Reactant(molecule, LayerType::NONE, amount, *this));
}

const ReactantSet& Mixture::getContent() const
{
	return content;
}

ContentInitializer Mixture::getContentInitializer() const
{
	return ContentInitializer(content);
}

Amount<Unit::MOLE> Mixture::getAmountOf(const Reactant& reactant) const
{
	return content.getAmountOf(reactant);
}

Amount<Unit::MOLE> Mixture::getAmountOf(const ReactantSet& reactantSet) const
{
	return content.getAmountOf(reactantSet);
}

Amount<Unit::MOLE> Mixture::getAmountOf(const Catalyst& catalyst) const
{
	return content.getAmountOf(catalyst);
}
