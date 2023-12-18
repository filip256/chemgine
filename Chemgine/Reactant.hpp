#pragma once

#include "Molecule.hpp"
#include "LayerType.hpp"
#include "Amount.hpp"

class Reactant
{
public:
	const Molecule molecule;
	mutable LayerType layer;
	mutable Amount<Unit::MOLE> amount;
	mutable bool isNew;

	Reactant(
		const Molecule& molecule,
		const LayerType layer,
		const Amount<Unit::MOLE> amount
	) noexcept;

	Reactant(const Reactant&) = default;
	Reactant(Reactant&&) = default;

	Amount<Unit::LITER> getVolumeAt(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure
	) const;

	bool operator==(const Reactant& other) const;
	bool operator!=(const Reactant& other) const;
};

class ReactantHash
{
public:
	size_t operator() (const Reactant& reactant) const;
};