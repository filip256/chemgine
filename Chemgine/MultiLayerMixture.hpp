#pragma once

#include "Mixture.hpp"
#include "LayerProperties.hpp"
#include "Atmosphere.hpp"
#include "Ref.hpp"

class MultiLayerMixture : public Mixture
{
protected:
	Amount<Unit::TORR> pressure = 0.0;
	Amount<Unit::MOLE> totalMoles = 0.0;
	Amount<Unit::GRAM> totalMass = 0.0;
	Amount<Unit::LITER> totalVolume = 0.0;

	const Amount<Unit::LITER> maxVolume;
	Ref<BaseContainer> overflowTarget;

	std::unordered_map<LayerType, LayerProperties> layers;

	bool tryCreateLayer(const LayerType layer);
	void addToLayer(const Reactant& reactant, const uint8_t revert = 1.0);
	void removeFromLayer(const Reactant& reactant);
	void add(const Amount<Unit::JOULE> heat, const LayerType layer);

	void removeNegligibles();
	void checkOverflow();

	LayerType getTopLayer() const;
	LayerType getBottomLayer() const;
	LayerType getLayerAbove(LayerType layer) const;
	LayerType getLayerBelow(LayerType layer) const;
	LayerType getClosestLayer(LayerType layer) const;

	MultiLayerMixture(const MultiLayerMixture&) = default;

public:
	MultiLayerMixture(
		const Ref<Atmosphere> atmosphere,
		const Amount<Unit::LITER> maxVolume,
		const Ref<BaseContainer> overflowTarget
	) noexcept;

	void add(const Reactant& reactant) override final;

	bool hasLayer(const LayerType layer) const;
	LayerType findLayerFor(const Reactant& reactant) const;

	Amount<Unit::LITER> getMaxVolume() const;

	Amount<Unit::TORR> getPressure() const override final;
	Amount<Unit::MOLE> getTotalMoles() const override final;
	Amount<Unit::GRAM> getTotalMass() const override final;
	Amount<Unit::LITER> getTotalVolume() const override final;

	const LayerProperties& getLayerProperties(const LayerType layer) const override final;
	Amount<Unit::JOULE_PER_MOLE_CELSIUS> getLayerHeatCapacity(const LayerType layer) const override final;
	Amount<Unit::JOULE_PER_MOLE> getLayerKineticEnergy(const LayerType layer) const override final;

	void copyContentTo(Ref<BaseContainer> destination, const Amount<Unit::LITER> volume, const LayerType sourceLayer) const;
	void moveContentTo(Ref<BaseContainer> destination, const Amount<Unit::LITER> volume, const LayerType sourceLayer);
};