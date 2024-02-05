#include "Reactant.hpp"
#include "Reactor.hpp"

Reactant::Reactant(
	const Molecule& molecule,
	const LayerType layer,
	const Amount<Unit::MOLE> amount,
	const Ref<Mixture> container
) noexcept :
	molecule(molecule),
	layer(layer),
	amount(amount),
	container(container)
{}

Amount<Unit::CELSIUS> Reactant::getTemperature() const
{
	return getLayerProperties().getTemperature();
}

Amount<Unit::GRAM> Reactant::getMass() const
{
	return amount.to<Unit::GRAM>(molecule.getMolarMass());
}

Amount<Unit::LITER> Reactant::getVolume() const
{
	return amount.to<Unit::LITER>(molecule.getMolarMass(), molecule.getDensityAt(getLayerProperties().getTemperature(), container->getPressure()));
}

Amount<Unit::JOULE_PER_MOLE_CELSIUS> Reactant::getHeatCapacity() const
{
	return molecule.getHeatCapacityAt(getLayerProperties().getTemperature(), container->getPressure());
}

Amount<Unit::JOULE_PER_MOLE> Reactant::getKineticEnergy() const
{
	return container->getLayerKineticEnergy(layer);
}

Amount<Unit::JOULE_PER_MOLE> Reactant::getStandaloneKineticEnergy() const
{
	const auto temp = getLayerProperties().getTemperature();
	return molecule.getHeatCapacityAt(temp, container->getPressure()).to<Unit::JOULE_PER_MOLE>(temp);
}

Ref<Mixture> Reactant::getContainer() const
{
	return container;
}

const LayerProperties& Reactant::getLayerProperties() const
{
	return container->getLayerProperties(layer);
}

Reactant Reactant::mutate(const Amount<Unit::MOLE> newAmount) const
{
	return Reactant(molecule, layer, newAmount, container);
}

Reactant Reactant::mutate(const Ref<Mixture> newContainer) const
{
	return Reactant(molecule, layer, amount, newContainer);
}

Reactant Reactant::mutate(const Amount<Unit::MOLE> newAmount, const Ref<Mixture> newContainer) const
{
	return Reactant(molecule, layer, newAmount, newContainer);
}

bool Reactant::operator== (const Reactant& other) const
{
	return this->layer == other.layer && this->molecule.getId() == other.molecule.getId();
}

bool Reactant::operator!= (const Reactant& other) const
{
	return this->layer != other.layer || this->molecule.getId() != other.molecule.getId();
}
