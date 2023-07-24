#include "MoleculeReactable.hpp"
#include "DataStore.hpp"

MoleculeReactable::MoleculeReactable(const ComponentIdType id) noexcept :
	id(id)
{}

const MolecularStructure& MoleculeReactable::getStructure() const
{
	return dataStore().molecules.at(id).getStructure();
}