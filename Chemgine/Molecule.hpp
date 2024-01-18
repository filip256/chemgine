#pragma once

#include "DataStoreAccessor.hpp"
#include "MoleculeData.hpp"
#include "Amount.hpp"

class Molecule
{
private:
	static DataStoreAccessor dataAccessor;

	const MoleculeIdType id;
	Amount<Unit::GRAM_PER_MOLE> molarMass;

public:
	Molecule(const MoleculeIdType id) noexcept;
	Molecule(MolecularStructure&& structure) noexcept;
	Molecule(const std::string& smiles) noexcept;

	MoleculeIdType getId() const;
	Amount<Unit::GRAM_PER_MOLE> getMolarMass() const;
	const MoleculeData& data() const;

	const MolecularStructure& getStructure() const;

	Amount<Unit::CELSIUS> getMeltingPointAt(const Amount<Unit::TORR> pressure) const;
	Amount<Unit::CELSIUS> getBoilingPointAt(const Amount<Unit::TORR> pressure) const;

	Amount<Unit::GRAM_PER_MILLILITER> getDensityAt(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure
	) const;

	Amount<Unit::JOULE_PER_MOLE> getHeatCapacityAt(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure
	) const;

	Amount<Unit::JOULE_PER_MOLE> getFusionHeatAt(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure
	) const;

	Amount<Unit::JOULE_PER_MOLE> getVaporizationHeatAt(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure
	) const;

	Amount<Unit::JOULE_PER_MOLE> getSublimationHeatAt(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure
	) const;

	Amount<Unit::JOULE_PER_MOLE> getLiquefactionHeatAt(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure
	) const;

	Amount<Unit::JOULE_PER_MOLE> getCondensationHeatAt(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure
	) const;

	Amount<Unit::JOULE_PER_MOLE> getDepositionHeatAt(
		const Amount<Unit::CELSIUS> temperature,
		const Amount<Unit::TORR> pressure
	) const;


	bool operator==(const Molecule& other) const;
	bool operator!=(const Molecule& other) const;

	static void setDataStore(const DataStore& dataStore);
};

class MoleculeHash
{
public:
    size_t operator() (const Molecule& molecule) const;
};