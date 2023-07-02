#include "BaseComponent.hpp"
#include "DataStore.hpp"
#include "Logger.hpp"

const DataStore* BaseComponent::sDataStore = nullptr;
size_t BaseComponent::instanceCount = 0;

BaseComponent::BaseComponent(const ComponentType type) noexcept :
	type(type)
{
	if (sDataStore == nullptr)
		Logger::fatal("No datastore was set for molecular components.");
}

void BaseComponent::setDataStore(const DataStore& dataStore)
{
	BaseComponent::sDataStore = &dataStore;
}

const DataStore& BaseComponent::getDataStore()
{
	if (sDataStore == nullptr)
		Logger::fatal("No datastore was set for molecular components.");
	return *sDataStore;
}

const DataStore& BaseComponent::dataStore() const
{
	return *BaseComponent::sDataStore;
}


bool BaseComponent::isAtomicType() const
{
	return !BaseComponent::isCompositeType(*this);
}

bool BaseComponent::isCompositeType() const
{
	return BaseComponent::isCompositeType(*this);
}

bool BaseComponent::isCompositeType(const BaseComponent& component)
{
	return 
		component.type == ComponentType::FUNCTIONAL ||
		component.type == ComponentType::BACKBONE;
}




void* BaseComponent::operator new(const size_t count)
{
	++instanceCount;
	return ::operator new(count);
}

void BaseComponent::operator delete(void* ptr)
{
	--instanceCount;
	return ::operator delete(ptr);
}