#include "BaseComponent.hpp"
#include "Logger.hpp"

const DataStore* BaseComponent::dataStore = nullptr;
size_t BaseComponent::instanceCount = 0;

BaseComponent::BaseComponent(const ComponentType type) noexcept :
	type(type)
{
	if (dataStore == nullptr)
		Logger::log("No datastore was set for molecular components.", LogType::FATAL);
}

void BaseComponent::setDataStore(const DataStore* const dataStore)
{
	BaseComponent::dataStore = dataStore;
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