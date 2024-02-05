#pragma once

#include "BaseContainer.hpp"
#include "ReactantSet.hpp"

/// <summary>
/// The simplest type of reactant container with internal storage.
/// </summary>
class Mixture : public BaseContainer
{
protected:
	ReactantSet content = ReactantSet(*this);

	Mixture(const Mixture&) noexcept;

public:
	Mixture() = default;
	Mixture(Mixture&&) = default;

	void add(const Reactant& reactant) override;
	void add(const Mixture& other);

	Amount<Unit::MOLE> getAmountOf(const Reactant& reactant) const;
	Amount<Unit::MOLE> getAmountOf(const ReactantSet& reactantSet) const;

	virtual Amount<Unit::TORR> getPressure() const = 0;
	virtual Amount<Unit::MOLE> getTotalMoles() const = 0;
	virtual Amount<Unit::GRAM> getTotalMass() const = 0;
	virtual Amount<Unit::LITER> getTotalVolume() const = 0;

	virtual const LayerProperties& getLayerProperties(const LayerType layer) const = 0;
	virtual Amount<Unit::JOULE_PER_MOLE_CELSIUS> getLayerHeatCapacity(const LayerType layer) const = 0;
	virtual Amount<Unit::JOULE_PER_MOLE> getLayerKineticEnergy(const LayerType layer) const = 0;

	friend class Reactant;
};