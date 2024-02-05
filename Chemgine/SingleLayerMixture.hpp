#pragma once

#include "Mixture.hpp"
#include "LayerProperties.hpp"
#include "Constants.hpp"
#include "Ref.hpp"

template<LayerType L>
class SingleLayerMixture : public Mixture
{
protected:
	Amount<Unit::TORR> pressure;
	const Amount<Unit::LITER> maxVolume;
	Ref<BaseContainer> overflowTarget = nullptr;
	LayerProperties layer;

	void addToLayer(const Reactant& reactant, const uint8_t revert = 1.0);
	void removeFromLayer(const Reactant& reactant);

	void removeNegligibles();
	void checkOverflow();

	void scaleToVolume(const Amount<Unit::LITER> volume);

public:
	SingleLayerMixture(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure,
		const std::vector<std::pair<Molecule, Amount<Unit::MOLE>>>& initContent,
		const Amount<Unit::LITER> maxVolume,
		const Ref<BaseContainer> overflowTarget
	) noexcept;

	const LayerProperties& getLayerProperties() const;
	Amount<Unit::JOULE_PER_MOLE_CELSIUS> getLayerHeatCapacity() const;
	Amount<Unit::JOULE_PER_MOLE> getLayerKineticEnergy() const;

	void setOverflowTarget(const Ref<BaseContainer> target);
	Ref<BaseContainer> getOverflowTarget() const;

	void add(const Reactant& reactant) override final;

	Amount<Unit::LITER> getMaxVolume() const;

	Amount<Unit::TORR> getPressure() const override final;
	Amount<Unit::MOLE> getTotalMoles() const override final;
	Amount<Unit::GRAM> getTotalMass() const override final;
	Amount<Unit::LITER> getTotalVolume() const override final;

	const LayerProperties& getLayerProperties(const LayerType l) const override final;
	Amount<Unit::JOULE_PER_MOLE_CELSIUS> getLayerHeatCapacity(const LayerType l) const override final;
	Amount<Unit::JOULE_PER_MOLE> getLayerKineticEnergy(const LayerType l) const override final;

	void copyContentTo(const Ref<BaseContainer> destination, const Amount<Unit::LITER> volume) const;
	void moveContentTo(const Ref<BaseContainer> destination, const Amount<Unit::LITER> volume);
};


template<LayerType L>
SingleLayerMixture<L>::SingleLayerMixture(
	const Amount<Unit::CELSIUS> temperature,
	const Amount<Unit::TORR> pressure,
	const std::vector<std::pair<Molecule, Amount<Unit::MOLE>>>& initContent,
	const Amount<Unit::LITER> maxVolume,
	const Ref<BaseContainer> overflowTarget
) noexcept :
	layer(temperature),
	pressure(pressure),
	maxVolume(maxVolume),
	overflowTarget(overflowTarget)
{
	for (size_t i = 0; i < initContent.size(); ++i)
		add(Reactant(initContent[i].first, L, initContent[i].second, *this));
	scaleToVolume(maxVolume);
}

template<LayerType L>
void SingleLayerMixture<L>::addToLayer(const Reactant& reactant, const uint8_t revert)
{
	layer.moles += reactant.amount * revert;
	layer.mass += reactant.getMass() * revert;
	layer.volume += reactant.getVolume() * revert;
}

template<LayerType L>
void SingleLayerMixture<L>::removeFromLayer(const Reactant& reactant)
{
	addToLayer(reactant, -1);
}

template<LayerType L>
void SingleLayerMixture<L>::removeNegligibles()
{
	for (const auto& r : content)
		if (r.amount < Constants::MOLAR_EXISTANCE_THRESHOLD)
			removeFromLayer(r);

	content.erase([](const auto& r) { return r.amount < Constants::MOLAR_EXISTANCE_THRESHOLD; });
}

template<LayerType L>
void SingleLayerMixture<L>::checkOverflow()
{
	if (maxVolume.isInfinity())
		return;

	const auto overflow = layer.volume - maxVolume;
	if (overflow <= 0.0)
		return;

	moveContentTo(overflowTarget, overflow);
}

template<LayerType L>
void SingleLayerMixture<L>::scaleToVolume(const Amount<Unit::LITER> volume)
{
	const auto difV = volume - layer.volume;
	if (difV == 0.0)
		return;

	copyContentTo(*this, difV);
}

template<LayerType L>
const LayerProperties& SingleLayerMixture<L>::getLayerProperties() const
{
	return layer;
}

template<LayerType L>
Amount<Unit::JOULE_PER_MOLE_CELSIUS> SingleLayerMixture<L>::getLayerHeatCapacity() const
{
	return getLayerProperties(L);
}

template<LayerType L>
Amount<Unit::JOULE_PER_MOLE> SingleLayerMixture<L>::getLayerKineticEnergy() const
{
	return getLayerKineticEnergy(L);
}

template<LayerType L>
void SingleLayerMixture<L>::setOverflowTarget(const Ref<BaseContainer> target)
{
	overflowTarget = target;
}

template<LayerType L>
Ref<BaseContainer> SingleLayerMixture<L>::getOverflowTarget() const
{
	return overflowTarget;
}

template<LayerType L>
void SingleLayerMixture<L>::add(const Reactant& reactant)
{
	if (reactant.layer != L)
		return;

	const auto& r = content.add(reactant);
	addToLayer(r);
}

template<LayerType L>
Amount<Unit::LITER> SingleLayerMixture<L>::getMaxVolume() const
{
	return maxVolume;
}

template<LayerType L>
Amount<Unit::TORR>  SingleLayerMixture<L>::getPressure() const
{
	return pressure;
}

template<LayerType L>
Amount<Unit::MOLE> SingleLayerMixture<L>::getTotalMoles() const
{
	return layer.moles;
}

template<LayerType L>
Amount<Unit::GRAM> SingleLayerMixture<L>::getTotalMass() const
{
	return layer.mass;
}

template<LayerType L>
Amount<Unit::LITER> SingleLayerMixture<L>::getTotalVolume() const
{
	return layer.volume;
}

template<LayerType L>
const LayerProperties& SingleLayerMixture<L>::getLayerProperties(const LayerType l) const
{
	return layer;
}

template<LayerType L>
Amount<Unit::JOULE_PER_MOLE_CELSIUS> SingleLayerMixture<L>::getLayerHeatCapacity(const LayerType l) const
{
	if (l != L)
		return 0.0;

	Amount<Unit::JOULE_PER_MOLE_CELSIUS> hC = 0.0;
	for (const auto& r : content)
	{
		hC += r.getHeatCapacity() * r.getMass().asStd();
	}

	return hC / layer.mass.asStd();
}

template<LayerType L>
Amount<Unit::JOULE_PER_MOLE> SingleLayerMixture<L>::getLayerKineticEnergy(const LayerType l) const
{
	return getLayerHeatCapacity(l).to<Unit::JOULE_PER_MOLE>(layer.temperature);
}

template<LayerType L>
void SingleLayerMixture<L>::copyContentTo(Ref<BaseContainer> destination, const Amount<Unit::LITER> volume) const
{
	if (destination.isSet() == false)
		return;

	// save and use initial volume
	const auto sourceVolume = layer.volume.asStd();

	for (const auto& r : content)
	{
		const auto molesToAdd = (r.amount / sourceVolume) * volume.asStd();
		destination->add(r.mutate(molesToAdd));
	}
}

template<LayerType L>
void SingleLayerMixture<L>::moveContentTo(Ref<BaseContainer> destination, Amount<Unit::LITER> volume)
{
	// save and use initial volume
	const auto sourceVolume = layer.volume.asStd();

	if (volume > sourceVolume)
		volume = sourceVolume;

	const bool hasDestination = destination.isSet();
	for (const auto& r : content)
	{
		const auto molesToAdd = (r.amount / sourceVolume) * volume.asStd();
		if(hasDestination)
			destination->add(r.mutate(molesToAdd));
		add(r.mutate(-molesToAdd));
	}
}
