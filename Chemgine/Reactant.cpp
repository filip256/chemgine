#include "Reactant.hpp"
#include "Reactor.hpp"
#include "Layer.hpp"

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

Amount<Unit::GRAM> Reactant::getMass() const
{
	return amount.to<Unit::GRAM>(molecule.getMolarMass());
}

Amount<Unit::LITER> Reactant::getVolume() const
{
	return amount
		.to<Unit::GRAM>(molecule.getMolarMass())
		.to<Unit::LITER>(getDensity());
}

Amount<Unit::GRAM_PER_MILLILITER> Reactant::getDensity() const
{
	return molecule.getDensityAt(getLayer().getTemperature(), container->getPressure());
}

Amount<Unit::CELSIUS> Reactant::getMeltingPoint() const
{
	return molecule.getMeltingPointAt(container->getPressure());
}

Amount<Unit::CELSIUS> Reactant::getBoilingPoint() const
{
	return molecule.getBoilingPointAt(container->getPressure());
}

Amount<Unit::JOULE_PER_MOLE_CELSIUS> Reactant::getHeatCapacity() const
{
	return molecule.getHeatCapacityAt(getLayer().getTemperature(), container->getPressure());
}

Amount<Unit::JOULE_PER_MOLE> Reactant::getKineticEnergy() const
{
	return container->getLayerKineticEnergy(layer);
}

Amount<Unit::JOULE_PER_MOLE> Reactant::getStandaloneKineticEnergy() const
{
	const auto temp = getLayer().getTemperature();
	return molecule.getHeatCapacityAt(temp, container->getPressure()).to<Unit::JOULE_PER_MOLE>(temp);
}

Amount<Unit::JOULE_PER_MOLE> Reactant::getLiquefactionHeat() const
{
	return molecule.getLiquefactionHeatAt(getLayer().getTemperature(), container->getPressure());
}

Amount<Unit::JOULE_PER_MOLE> Reactant::getFusionHeat() const
{
	return molecule.getFusionHeatAt(getLayer().getTemperature(), container->getPressure());
}

Amount<Unit::JOULE_PER_MOLE> Reactant::getVaporizationHeat() const
{
	return molecule.getVaporizationHeatAt(getLayer().getTemperature(), container->getPressure());
}

Amount<Unit::JOULE_PER_MOLE> Reactant::getCondensationHeat() const
{
	return molecule.getCondensationHeatAt(getLayer().getTemperature(), container->getPressure());
}

Amount<Unit::JOULE_PER_MOLE> Reactant::getSublimationHeat() const
{
	return molecule.getSublimationHeatAt(getLayer().getTemperature(), container->getPressure());
}

Amount<Unit::JOULE_PER_MOLE> Reactant::getDepositionHeat() const
{
	return molecule.getDepositionHeatAt(getLayer().getTemperature(), container->getPressure());
}

Ref<Mixture> Reactant::getContainer() const
{
	return container;
}

const Layer& Reactant::getLayer() const
{
	return container->getLayer(layer);
}

Amount<Unit::CELSIUS> Reactant::getLayerTemperature() const
{
	return getLayer().getTemperature();
}

AggregationType Reactant::getAggregation() const
{
	return getAggregation(getLayerTemperature());
}

AggregationType Reactant::getAggregation(const Amount<Unit::CELSIUS> temperature) const
{
	return molecule.getAggregationAt(temperature, container->getPressure());
}

Reactant Reactant::mutate(const Amount<Unit::MOLE> newAmount) const
{
	return Reactant(molecule, layer, newAmount, container);
}

Reactant Reactant::mutate(const Ref<Mixture> newContainer) const
{
	return Reactant(molecule, layer, amount, newContainer);
}

Reactant Reactant::mutate(const LayerType newLayer) const
{
	return Reactant(molecule, newLayer, amount, container);
}

Reactant Reactant::mutate(const Amount<Unit::MOLE> newAmount, const Ref<Mixture> newContainer) const
{
	return Reactant(molecule, layer, newAmount, newContainer);
}

Reactant Reactant::mutate(const Amount<Unit::MOLE> newAmount, const LayerType newLayer) const
{
	return Reactant(molecule, newLayer, newAmount);
}

Reactant Reactant::mutate(const Amount<Unit::MOLE> newAmount, const Ref<Mixture> newContainer, const LayerType newLayer) const
{
	return Reactant(molecule, newLayer, newAmount, newContainer);
}

bool Reactant::operator== (const Reactant& other) const
{
	return this->layer == other.layer && this->molecule.getId() == other.molecule.getId();
}

bool Reactant::operator!= (const Reactant& other) const
{
	return this->layer != other.layer || this->molecule.getId() != other.molecule.getId();
}
