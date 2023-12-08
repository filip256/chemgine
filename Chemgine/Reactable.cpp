#include "Reactable.hpp"
#include "DataStore.hpp"

DataStoreAccessor Reactable::dataAccessor = DataStoreAccessor();

Reactable::Reactable(const ComponentIdType id, const bool isGeneric) noexcept:
	id(id),
	isGeneric(isGeneric)
{
	dataAccessor.crashIfUninitialized();
}

void Reactable::setDataStore(const DataStore& dataStore)
{
	dataAccessor.set(dataStore);
}

const DataStore& Reactable::dataStore() const
{
	return dataAccessor.get();
}

const ComponentIdType Reactable::getId() const
{
	return id;
}


const MolecularStructure& Reactable::getStructure() const
{
	if(isGeneric)
		return dataAccessor.get().functionalGroups.at(id).getStructure();

	return dataAccessor.get().molecules.at(id).getStructure();
}

std::unordered_map<c_size, c_size> Reactable::matchWith(const MolecularStructure& structure) const
{
	const auto& thisStructure = getStructure();
	const auto map = structure.mapTo(thisStructure, true);
	if (map.size() == thisStructure.componentCount())
		return map;

	return std::unordered_map<c_size, c_size>();
}

Reactable Reactable::get(const std::string& smiles)
{
	const MolecularStructure structure(smiles);

	if (structure.isComplete())
	{
		const auto idx = dataAccessor.get().molecules.findFirst(structure);
		return Reactable(idx == DataStore::npos ? 0 : dataAccessor.get().molecules[idx].id, false);
	}
	else
	{
		const auto idx = dataAccessor.get().functionalGroups.findFirst(structure);
		return Reactable(idx == DataStore::npos ? 0 : dataAccessor.get().functionalGroups[idx].id, true);
	}

}