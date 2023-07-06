#include "OrganicMolecule.hpp"
#include "DataStore.hpp"

DataStoreAccessor OrganicMolecule::dataAccessor = DataStoreAccessor();

OrganicMolecule::OrganicMolecule(const MoleculeIdType id) :
	id(id)
{
	dataAccessor.crashIfUninitialized();
}

const OrganicMoleculeData& OrganicMolecule::data() const
{
	return dataAccessor.get().molecules[id];
}