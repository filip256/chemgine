#include "BaseLabwareComponent.hpp"
#include "DataStore.hpp"

size_t BaseLabwareComponent::instanceCount = 0;
DataStoreAccessor BaseLabwareComponent::dataAccessor = DataStoreAccessor();

BaseLabwareComponent::BaseLabwareComponent(const LabwareIdType id) noexcept :
	data(dataAccessor.getSafe().labware.at(id))
{}


void BaseLabwareComponent::setDataStore(const DataStore& dataStore)
{
	dataAccessor.set(dataStore);
}

#ifndef NDEBUG
void* BaseLabwareComponent::operator new(const size_t count)
{
	++instanceCount;
	return ::operator new(count);
}

void BaseLabwareComponent::operator delete(void* ptr)
{
	--instanceCount;
	return ::operator delete(ptr);
}
#endif