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

Amount<Unit::GRAM> Reactant::getMass() const
{
	return amount.to<Unit::GRAM>(molecule.getMolarMass());
}

Amount<Unit::LITER> Reactant::getVolumeAt(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure
) const
{
	return amount.to<Unit::LITER>(molecule.getMolarMass(), molecule.getDensityAt(temperature, pressure));
}

bool Reactant::operator== (const Reactant& other) const
{
	return this->layer == other.layer && this->molecule.getId() == other.molecule.getId();
}

bool Reactant::operator!= (const Reactant& other) const
{
	return this->layer != other.layer || this->molecule.getId() != other.molecule.getId();
}


size_t ReactantHash::operator() (const Reactant& reactant) const
{
	return PairHash()(reactant.molecule.getId(), toIndex(reactant.layer));
}