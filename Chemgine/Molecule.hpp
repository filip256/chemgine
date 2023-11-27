#pragma once

#include "DataStoreAccessor.hpp"
#include "OrganicMoleculeData.hpp"

class Molecule
{
private:
	static DataStoreAccessor dataAccessor;

	const MoleculeIdType id;

public:
	Molecule(const MoleculeIdType id) noexcept;
	Molecule(MolecularStructure&& structure) noexcept;
	Molecule(const std::string& smiles) noexcept;

	MoleculeIdType getId() const;
	const OrganicMoleculeData& data() const;

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