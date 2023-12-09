#pragma once

#include "DataStoreAccessor.hpp"
#include "MoleculeData.hpp"
#include "Amount.hpp"

class Molecule
{
private:
	static DataStoreAccessor dataAccessor;

	const MoleculeIdType id;
	const Amount<Unit::GRAM_PER_MOLE> molarMass;

public:
	Molecule(const MoleculeIdType id) noexcept;
	Molecule(MolecularStructure&& structure) noexcept;
	Molecule(const std::string& smiles) noexcept;

	MoleculeIdType getId() const;
	Amount<Unit::GRAM_PER_MOLE> getMolarMass() const;
	const MoleculeData& data() const;

	const MolecularStructure& getStructure() const;

	bool operator==(const Molecule& other) const;
	bool operator!=(const Molecule& other) const;

	static void setDataStore(const DataStore& dataStore);
};

class MoleculeHash
{
public:
    size_t operator() (const Molecule& molecule) const;
};