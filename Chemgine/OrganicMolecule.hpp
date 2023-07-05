#pragma once

#include "OrganicMoleculeData.hpp"

class OrganicMolecule
{
private:
	static DataStoreAccessor dataAccessor;

	const MoleculeIdType id;

public:
	OrganicMolecule(const MoleculeIdType id);

	const OrganicMoleculeData& data() const;
};