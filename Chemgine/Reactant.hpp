#pragma once

#include "Molecule.hpp"
#include "LayerType.hpp"
#include "Amount.hpp"
#include "HashCombine.hpp"
#include "Ref.hpp"
#include "AggregationType.hpp"

class Mixture;
class Layer;

class Reactant
{
private:
	Ref<Mixture> container;

public:
	mutable bool isNew = true;
	mutable LayerType layer;
	const Molecule molecule;
	mutable Amount<Unit::MOLE> amount;

	Reactant(
		const Molecule& molecule,
		const LayerType layer,
		const Amount<Unit::MOLE> amount,
		const Ref<Mixture> container = Ref<Mixture>::nullRef
	) noexcept;

	Reactant(const Reactant&) = default;
	Reactant(Reactant&&) = default;

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

	Ref<Mixture> getContainer() const;
	const Layer& getLayer() const;
	Amount<Unit::CELSIUS> getLayerTemperature() const;

	AggregationType getAggregation() const;
	AggregationType getAggregation(const Amount<Unit::CELSIUS> temperature) const;

	Reactant mutate(const Amount<Unit::MOLE> newAmount) const;
	Reactant mutate(const Ref<Mixture> newContainer) const;
	Reactant mutate(const LayerType newLayer) const;
	Reactant mutate(const Amount<Unit::MOLE> newAmount, const Ref<Mixture> newContainer) const;
	Reactant mutate(const Amount<Unit::MOLE> newAmount, const LayerType newLayer) const;
	Reactant mutate(const Amount<Unit::MOLE> newAmount, const Ref<Mixture> newContainer, const LayerType newLayer) const;

	bool operator==(const Reactant& other) const;
	bool operator!=(const Reactant& other) const;
};


template<>
struct std::hash<Reactant>
{
	size_t operator() (const Reactant& reactant) const
	{
		return hashCombine(reactant.molecule.getId(), toIndex(reactant.layer));
	}
};