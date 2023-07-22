#include "BackboneReactable.hpp"
#include "DataStore.hpp"

BackboneReactable::BackboneReactable(const ComponentIdType id) noexcept :
	id(id)
{}

const MolecularStructure& BackboneReactable::getStructure() const
{
	return dataStore().backbones.at(id).getStructure();
}