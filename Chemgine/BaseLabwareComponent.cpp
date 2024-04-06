#include "BaseLabwareComponent.hpp"
#include "DataStore.hpp"

size_t BaseLabwareComponent::instanceCount = 0;
DataStoreAccessor BaseLabwareComponent::dataAccessor = DataStoreAccessor();

BaseLabwareComponent::BaseLabwareComponent(const LabwareId id) noexcept :
	id(id),
	data(dataAccessor.getSafe().labware.at(id))
{}

const DataStore& BaseLabwareComponent::dataStore() const
{
	return dataAccessor.getSafe();
}

void BaseLabwareComponent::setDataStore(const DataStore& dataStore)
{
	dataAccessor.set(dataStore);
}

bool BaseLabwareComponent::isFlask() const
{
	return data.type == LabwareType::FLASK;
}

bool BaseLabwareComponent::isAdaptor() const
{
	return data.type == LabwareType::ADAPTOR;
}

bool BaseLabwareComponent::isHeatsource() const
{
	return data.type == LabwareType::HEATSOURCE;
}

bool BaseLabwareComponent::isContainer() const
{
	return false;
}

bool BaseLabwareComponent::tryConnect(BaseLabwareComponent& other)
{
	return false;
}

void BaseLabwareComponent::disconnect(Ref<BaseContainer> dump, const BaseLabwareComponent& other)
{}

void BaseLabwareComponent::tick(const Amount<Unit::SECOND> timespan)
{}

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
