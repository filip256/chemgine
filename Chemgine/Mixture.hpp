#pragma once

#include "BaseContainer.hpp"
#include "ReactantSet.hpp"
#include "ContentInitializer.hpp"
#include "Ref.hpp"

class Layer;

/// <summary>
/// The simplest type of reactant container with internal storage.
/// </summary>
class Mixture : public BaseContainer
{
protected:
	ReactantSet content = ReactantSet(*this);

	Mixture(const Mixture&) noexcept;

	virtual void add(const Amount<Unit::JOULE> heat, const LayerType layer) = 0;

	virtual LayerType findLayerFor(const Reactant& reactant) const = 0;

public:
	Mixture() = default;
	Mixture(Mixture&&) = default;

	void add(const Reactant& reactant) override;
	void add(const Mixture& other);
	virtual void add(const Molecule& molecule, const Amount<Unit::MOLE> amount);

	const ReactantSet& getContent() const;
	ContentInitializer getContentInitializer() const;

	Amount<Unit::MOLE> getAmountOf(const Reactant& reactant) const;
	Amount<Unit::MOLE> getAmountOf(const ReactantSet& reactantSet) const;

	virtual Amount<Unit::TORR> getPressure() const = 0;
	virtual Amount<Unit::MOLE> getTotalMoles() const = 0;
	virtual Amount<Unit::GRAM> getTotalMass() const = 0;
	virtual Amount<Unit::LITER> getTotalVolume() const = 0;

	virtual const Layer& getLayer(const LayerType layer) const = 0;
	virtual Amount<Unit::CELSIUS> getLayerTemperature(const LayerType l) const = 0;
	virtual Amount<Unit::JOULE_PER_MOLE_CELSIUS> getLayerHeatCapacity(const LayerType layer) const = 0;
	virtual Amount<Unit::JOULE_PER_CELSIUS> getLayerTotalHeatCapacity(const LayerType layer) const = 0;
	virtual Amount<Unit::JOULE_PER_MOLE> getLayerKineticEnergy(const LayerType layer) const = 0;
	virtual Polarity getLayerPolarity(const LayerType layer) const = 0;
	virtual Color getLayerColor(const LayerType layer) const = 0;

	virtual bool isEmpty() const = 0;

	using OverflowTargetId = uint8_t;
	virtual Ref<BaseContainer> getOverflowTarget(const OverflowTargetId id) const = 0;
	virtual void setOverflowTarget(const Ref<BaseContainer> target, const OverflowTargetId id) = 0;

	friend class Layer;
};
