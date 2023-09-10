#include "MoleculeReactable.hpp"
#include "DataStore.hpp"

MoleculeReactable::MoleculeReactable(const ComponentIdType id) noexcept :
	id(id)
{}

const MolecularStructure& MoleculeReactable::getStructure() const
{
	return dataStore().molecules.at(id).getStructure();
}

std::unordered_map<c_size, c_size> MoleculeReactable::matchWith(const MolecularStructure& structure) const
{
	const auto& thisStructure = getStructure();
	if (thisStructure.componentCount() != structure.componentCount() ||
		thisStructure.getHydrogenCount() != structure.getHydrogenCount())
		return std::unordered_map<c_size, c_size>();

	const auto map = structure.mapTo(thisStructure, false);
	if (map.size() == thisStructure.componentCount())
		return map;

	return std::unordered_map<c_size, c_size>();
}