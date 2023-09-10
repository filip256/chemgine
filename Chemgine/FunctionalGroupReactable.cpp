#include "FunctionalGroupReactable.hpp"
#include "DataStore.hpp"

FunctionalGroupReactable::FunctionalGroupReactable(const ComponentIdType id) noexcept :
	id(id)
{}

const MolecularStructure& FunctionalGroupReactable::getStructure() const
{
	return dataStore().functionalGroups.at(id).getStructure();
}

std::unordered_map<c_size, c_size> FunctionalGroupReactable::matchWith(const MolecularStructure& structure) const
{
	const auto& thisStructure = getStructure();
	const auto map = structure.mapTo(thisStructure, true);
	if (map.size() == thisStructure.componentCount())
		return map;

	return std::unordered_map<c_size, c_size>();
}