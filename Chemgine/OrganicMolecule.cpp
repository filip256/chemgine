#include "OrganicMolecule.hpp"

OrganicMolecule::OrganicMolecule(const MoleculeIdType id) :
	id(id)
{
	dataAccessor.crashIfUninitialized();
}

const OrganicMoleculeData& OrganicMolecule::data() const
{
	return dataAccessor.get().molecules[id];
}