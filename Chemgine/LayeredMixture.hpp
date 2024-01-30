#pragma once

#include "Mixture.hpp"

/// <summary>
/// Interface for layered mixtures.
/// </summary>
class LayeredMixture : public Mixture
{
protected:
	LayeredMixture() = default;

public:
	virtual Amount<Unit::TORR> getPressure() const = 0;
	virtual Amount<Unit::MOLE> getTotalMoles() const = 0;
	virtual Amount<Unit::GRAM> getTotalMass() const = 0;
	virtual Amount<Unit::LITER> getTotalVolume() const = 0;

	virtual const LayerProperties& getLayerProperties(const LayerType layer) const = 0;
	virtual Amount<Unit::JOULE_PER_MOLE_CELSIUS> getLayerHeatCapacity(const LayerType layer) const = 0;
	virtual Amount<Unit::JOULE_PER_MOLE> getLayerKineticEnergy(const LayerType layer) const = 0;
};