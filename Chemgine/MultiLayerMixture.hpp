#pragma once

#include "LayeredMixture.hpp"
#include "LayerProperties.hpp"
#include "SingleLayerMixture.hpp"

class MultiLayerMixture : public LayeredMixture
{
protected:
	Amount<Unit::TORR> pressure;
	Amount<Unit::MOLE> totalMoles;
	Amount<Unit::GRAM> totalMass;
	Amount<Unit::LITER> totalVolume;

	const Amount<Unit::LITER> maxVolume;
	Mixture* overflowTarget = nullptr;

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

public:
	MultiLayerMixture(
		const SingleLayerMixture<LayerType::GASEOUS>& atmosphere,
		const Amount<Unit::LITER> maxVolume,
		Mixture* overflowTarget
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

	void copyContentTo(Mixture* destination, const Amount<Unit::LITER> volume, const LayerType sourceLayer) const;
	void moveContentTo(Mixture* destination, const Amount<Unit::LITER> volume, const LayerType sourceLayer);
};