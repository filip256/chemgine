#include "ReactableFactory.hpp"
#include "DataStore.hpp"
#include "FunctionalGroupReactable.hpp"
#include "BackboneReactable.hpp"

ReactableFactory::ReactableFactory() noexcept
{
	Reactable::dataAccessor.crashIfUninitialized();
}

const Reactable* ReactableFactory::get(const std::string& smiles) const
{
	const auto& data = Reactable::dataAccessor.get();
	size_t result = data.functionalGroups.findFirst(smiles);
	if (result != DataStore::npos)
		return new FunctionalGroupReactable(data.functionalGroups[result].id);

	result = data.backbones.findFirst(smiles);
	if (result != DataStore::npos)
		return new BackboneReactable(data.backbones[result].id);

	return nullptr;
}

const Reactable* ReactableFactory::get(const ComponentIdType id) const
{
	if(Reactable::dataAccessor.get().functionalGroups.contains(id))
		return new FunctionalGroupReactable(id);

	if (Reactable::dataAccessor.get().functionalGroups.contains(id))
		return new BackboneReactable(id);

	return nullptr;
}

void ReactableFactory::setDataStore(const DataStore& dataStore)
{
	Reactable::setDataStore(dataStore);
}