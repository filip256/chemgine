#include "FunctionalGroupReactable.hpp"
#include "DataStore.hpp"

FunctionalGroupReactable::FunctionalGroupReactable(const ComponentIdType id) noexcept :
	id(id)
{}

const MolecularStructure& FunctionalGroupReactable::getStructure() const
{
	return dataStore().backbones.at(id).getStructure();
}