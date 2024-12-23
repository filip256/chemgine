#pragma once

#include "Molecule.hpp"
#include "LayerType.hpp"
#include "Amount.hpp"
#include "HashUtils.hpp"
#include "Ref.hpp"
#include "AggregationType.hpp"

class Reactant;

class ReactantId
{
public:
	const LayerType layer;
	const MoleculeId moleculeId;

	ReactantId(
		const MoleculeId moleculeId,
		const LayerType layer
	) noexcept;
	ReactantId(const Reactant&) noexcept;
	ReactantId(const ReactantId& reactant) = default;

	bool operator==(const ReactantId& other) const;
	bool operator!=(const ReactantId& other) const;
};

template<>
struct std::hash<ReactantId>
{
	size_t operator() (const ReactantId& id) const
	{
		return Utils::hashCombine(id.moleculeId, toIndex(id.layer));
	}
};



class Mixture;
class Layer;

class Reactant
{
private:
	Ref<Mixture> container;

public:
	bool isNew = true;
	const LayerType layer;
	const Molecule molecule;
	Amount<Unit::MOLE> amount;

	Reactant(
		const Molecule& molecule,
		const LayerType layer,
		const Amount<Unit::MOLE> amount,
		const Ref<Mixture> container = nullRef
	) noexcept;

	Reactant(const Reactant&) = default;
	Reactant(Reactant&&) = default;

	ReactantId getId() const;

	Amount<Unit::GRAM> getMass() const;
	Amount<Unit::LITER> getVolume() const;
	Amount<Unit::GRAM_PER_MILLILITER> getDensity() const;
	Amount<Unit::CELSIUS> getMeltingPoint() const;
	Amount<Unit::CELSIUS> getBoilingPoint() const;
	Amount<Unit::JOULE_PER_MOLE_CELSIUS> getHeatCapacity() const;
	Amount<Unit::JOULE_PER_MOLE> getKineticEnergy() const;
	Amount<Unit::JOULE_PER_MOLE> getStandaloneKineticEnergy() const;
	Amount<Unit::JOULE_PER_MOLE> getLiquefactionHeat() const;
	Amount<Unit::JOULE_PER_MOLE> getFusionHeat() const;
	Amount<Unit::JOULE_PER_MOLE> getVaporizationHeat() const;
	Amount<Unit::JOULE_PER_MOLE> getCondensationHeat() const;
	Amount<Unit::JOULE_PER_MOLE> getSublimationHeat() const;
	Amount<Unit::JOULE_PER_MOLE> getDepositionHeat() const;
	Amount<Unit::MOLE_RATIO> getSolubilityIn(const Polarity& solventPolarity) const;

	Ref<Mixture> getContainer() const;
	const Layer& getLayer() const;
	Amount<Unit::CELSIUS> getLayerTemperature() const;

	AggregationType getAggregation() const;
	AggregationType getAggregationAt(const Amount<Unit::CELSIUS> temperature) const;

	Reactant mutate(const Amount<Unit::MOLE> newAmount) const;
	Reactant mutate(const Ref<Mixture> newContainer) const;
	Reactant mutate(const LayerType newLayer) const;
	Reactant mutate(const Amount<Unit::MOLE> newAmount, const Ref<Mixture> newContainer) const;
	Reactant mutate(const Amount<Unit::MOLE> newAmount, const LayerType newLayer) const;
	Reactant mutate(const Amount<Unit::MOLE> newAmount, const Ref<Mixture> newContainer, const LayerType newLayer) const;
};
