#include "Molecule.hpp"
#include "DataStore.hpp"
#include "Amount.hpp"

DataStoreAccessor Molecule::dataAccessor = DataStoreAccessor();

Molecule::Molecule(const MoleculeIdType id) noexcept:
	id(id)
{
	dataAccessor.crashIfUninitialized();
}

Molecule::Molecule(MolecularStructure&& structure) noexcept:
	id(dataAccessor.getSafe().molecules.findOrAdd(std::move(structure)))
{
}

Molecule::Molecule(const std::string& smiles) noexcept:
	Molecule(MolecularStructure(smiles))
{
}

void Molecule::setDataStore(const DataStore& dataStore)
{
	dataAccessor.set(dataStore);
}

MoleculeIdType Molecule::getId() const
{
	return id;
}

const OrganicMoleculeData& Molecule::data() const
{
	return dataAccessor.get().molecules.at(id);
}

const MolecularStructure& Molecule::getStructure() const
{
	return data().getStructure();
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