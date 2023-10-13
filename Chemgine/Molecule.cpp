#include "Molecule.hpp"
#include "DataStore.hpp"

DataStoreAccessor Molecule::dataAccessor = DataStoreAccessor();

Molecule::Molecule(const MoleculeIdType id) :
	id(id)
{
	dataAccessor.crashIfUninitialized();
}

Molecule::Molecule(MolecularStructure&& structure) :
	id(dataAccessor.getSafe().molecules.findOrAdd(std::move(structure)))
{
}

MoleculeIdType Molecule::getId() const
{
	return id;
}

const OrganicMoleculeData& Molecule::data() const
{
	return dataAccessor.get().molecules.at(id);
}

bool Molecule::operator==(const Molecule& other) const
{
	return this->id == other.id;
}

bool Molecule::operator!=(const Molecule& other) const
{
	return this->id != other.id;
}


size_t MoleculeHash::operator() (const Molecule& molecule) const
{
	return std::hash<MoleculeIdType>()(molecule.getId());
}