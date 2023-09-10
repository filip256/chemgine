#include "BackboneReactable.hpp"
#include "DataStore.hpp"

BackboneReactable::BackboneReactable(const ComponentIdType id) noexcept :
	id(id)
{}

const MolecularStructure& BackboneReactable::getStructure() const
{
	return dataStore().backbones.at(id).getStructure();
}

std::unordered_map<c_size, c_size> BackboneReactable::matchWith(const MolecularStructure& structure) const
{
	const auto& thisStructure = getStructure();
	const auto map = structure.maximalMapTo(thisStructure).first;
	if (map.size() == thisStructure.componentCount())
		return map;

	return std::unordered_map<c_size, c_size>();
}