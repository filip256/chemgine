#pragma once

#include "DataStoreAccessor.hpp"
#include "OrganicMoleculeData.hpp"

class Molecule
{
private:
	static DataStoreAccessor dataAccessor;

	const MoleculeIdType id;

public:
	Molecule(const MoleculeIdType id);
	Molecule(MolecularStructure&& structure);

	MoleculeIdType getId() const;
	const OrganicMoleculeData& data() const;

	bool operator==(const Molecule& other) const;
	bool operator!=(const Molecule& other) const;
};

class MoleculeHash
{
public:
    size_t operator() (const Molecule& molecule) const;
};