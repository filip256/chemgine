#pragma once

#include "Molecule.hpp"
#include "LayerType.hpp"
#include "Amount.hpp"
#include "LayerProperties.hpp"
#include "HashCombine.hpp"

class LayeredMixture;

class Reactant
{
private:
	mutable const LayeredMixture* container = nullptr;

public:
	mutable bool isNew = true;
	mutable LayerType layer;
	const Molecule molecule;
	mutable Amount<Unit::MOLE> amount;

	Reactant(
		const Molecule& molecule,
		const LayerType layer,
		const Amount<Unit::MOLE> amount
	) noexcept;

	Reactant(
		const Molecule& molecule,
		const LayerType layer,
		const Amount<Unit::MOLE> amount,
		const LayeredMixture& container
	) noexcept;

	Reactant(const Reactant&) = default;
	Reactant(Reactant&&) = default;

	Amount<Unit::CELSIUS> getTemperature() const;
	Amount<Unit::GRAM> getMass() const;
	Amount<Unit::LITER> getVolume() const;
	Amount<Unit::JOULE_PER_MOLE_CELSIUS> getHeatCapacity() const;
	Amount<Unit::JOULE_PER_MOLE> getKineticEnergy() const;
	Amount<Unit::JOULE_PER_MOLE> getStandaloneKineticEnergy() const;

	void setContainer(const LayeredMixture& container) const;
	const LayeredMixture* getContainer() const;
	const LayerProperties& getLayerProperties() const;

	void markAsNew() const;

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