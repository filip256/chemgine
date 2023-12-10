#include "Reactant.hpp"
#include "PairHash.hpp"

Reactant::Reactant(
	const Molecule& molecule,
	const LayerType layer,
	const Amount<Unit::MOLE> amount
) noexcept :
	molecule(molecule),
	layer(layer),
	amount(amount),
	isNew(true)
{}

Amount<Unit::LITER> Reactant::getVolumeAt(const Amount<Unit::CELSIUS> temperature) const
{
	return amount.to<Unit::LITER>(molecule.getMolarMass(), molecule.data().getDensityAt(temperature));
}

bool Reactant::operator== (const Reactant& other) const
{
	return this->molecule.getId() == other.molecule.getId();
}

bool Reactant::operator!= (const Reactant& other) const
{
	return this->molecule.getId() != other.molecule.getId();
}


size_t ReactantHash::operator() (const Reactant& reactant) const
{
	return reactant.molecule.getId();
}