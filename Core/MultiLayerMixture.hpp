#pragma once

#include "Mixture.hpp"
#include "Layer.hpp"
#include "AtmosphereMixture.hpp"
#include "DumpContainer.hpp"

#include <map>

class MultiLayerMixture : public Mixture
{
protected:
	Amount<Unit::TORR> pressure = 0.0;
	Amount<Unit::MOLE> totalMoles = 0.0;
	Amount<Unit::GRAM> totalMass = 0.0;
	Amount<Unit::LITER> totalVolume = 0.0;

	const Amount<Unit::LITER> maxVolume;
	Ref<ContainerBase> overflowTarget = DumpContainer::GlobalDumpContainer;

	std::map<LayerType, Layer> layers;

	bool tryCreateLayer(const LayerType layer);
	void addToLayer(const Reactant& reactant);
	void add(const Quantity<Joule> heat, const LayerType layer) override final;

	void removeNegligibles();
	void checkOverflow();

	LayerType getTopLayer() const;
	LayerType getBottomLayer() const;
	LayerType getLayerAbove(LayerType layer) const;
	LayerType getLayerBelow(LayerType layer) const;
	LayerType getClosestLayer(LayerType layer) const;
	bool areAdjacentLayers(LayerType layer1, LayerType layer2) const;

	LayerType findLayerFor(const Reactant& reactant) const override final;

	MultiLayerMixture(const MultiLayerMixture&) noexcept;

public:
	MultiLayerMixture(
		const Ref<AtmosphereMixture> atmosphere,
		const Amount<Unit::LITER> maxVolume,
		const Ref<ContainerBase> overflowTarget
	) noexcept;

	void add(const Reactant& reactant) override final;
	void add(const Molecule& molecule, const Amount<Unit::MOLE> amount) override;
	void add(const Quantity<Joule> heat) override;

	bool hasLayer(const LayerType layer) const;

	Amount<Unit::LITER> getMaxVolume() const;

	Amount<Unit::TORR> getPressure() const override final;
	Amount<Unit::MOLE> getTotalMoles() const override final;
	Quantity<Gram> getTotalMass() const override final;
	Amount<Unit::LITER> getTotalVolume() const override final;

	const Layer& getLayer(const LayerType layer) const override final;
	Amount<Unit::CELSIUS> getLayerTemperature(const LayerType layer) const override final;
	Amount<Unit::JOULE_PER_MOLE_CELSIUS> getLayerHeatCapacity(const LayerType layer) const override final;
	Amount<Unit::JOULE_PER_CELSIUS> getLayerTotalHeatCapacity(const LayerType layer) const override final;
	Amount<Unit::JOULE_PER_MOLE> getLayerKineticEnergy(const LayerType layer) const override final;
	Polarity getLayerPolarity(const LayerType layer) const override final;
	Color getLayerColor(const LayerType layer) const override final;

	bool isEmpty() const override final;

	Ref<ContainerBase> getOverflowTarget() const override final;
	void setOverflowTarget(const Ref<ContainerBase> target) override final;

	using LayerDownIterator = std::map<LayerType, Layer>::const_iterator;
	LayerDownIterator getLayersDownBegin() const;
	LayerDownIterator getLayersDownEnd() const;

	using LayerUpIterator = std::map<LayerType, Layer>::const_reverse_iterator;
	LayerUpIterator getLayersUpBegin() const;
	LayerUpIterator getLayersUpEnd() const;

	void copyContentTo(Ref<ContainerBase> destination, const Amount<Unit::LITER> volume, const LayerType sourceLayer) const;
	void moveContentTo(Ref<ContainerBase> destination, const Amount<Unit::LITER> volume, const LayerType sourceLayer);

	MultiLayerMixture makeCopy() const;
};
