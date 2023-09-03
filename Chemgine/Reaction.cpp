#include "Reaction.hpp"
#include "DataStore.hpp"

DataStoreAccessor Reaction::dataAccessor = DataStoreAccessor();

Reaction::Reaction(const ReactionIdType id) noexcept :
	id(id)
{}

void Reaction::setDataStore(const DataStore& dataStore)
{
	dataAccessor.set(dataStore);
}

ReactionIdType Reaction::getId() const
{
	return id;
}

const ReactionData& Reaction::data() const
{
	return dataAccessor.get().reactions[id];
}