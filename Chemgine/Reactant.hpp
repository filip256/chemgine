#pragma once

#include "Molecule.hpp"
#include "LayerType.hpp"
#include "Amount.hpp"
#include "LayerProperties.hpp"
#include "HashCombine.hpp"
#include "Ref.hpp"

class Mixture;

class Reactant
{
private:
	const Ref<Mixture> container;

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

	Amount<Unit::CELSIUS> getTemperature() const;
	Amount<Unit::GRAM> getMass() const;
	Amount<Unit::LITER> getVolume() const;
	Amount<Unit::JOULE_PER_MOLE_CELSIUS> getHeatCapacity() const;
	Amount<Unit::JOULE_PER_MOLE> getKineticEnergy() const;
	Amount<Unit::JOULE_PER_MOLE> getStandaloneKineticEnergy() const;

	Ref<Mixture> getContainer() const;
	const LayerProperties& getLayerProperties() const;

	Reactant mutate(const Amount<Unit::MOLE> newAmount) const;
	Reactant mutate(const Ref<Mixture> newContainer) const;
	Reactant mutate(const Amount<Unit::MOLE> newAmount, const Ref<Mixture> newContainer) const;

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