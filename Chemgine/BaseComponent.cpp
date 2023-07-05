#include "BaseComponent.hpp"
#include "DataStore.hpp"
#include "Logger.hpp"

size_t BaseComponent::instanceCount = 0;

BaseComponent::BaseComponent(const ComponentIdType id, const ComponentType type) noexcept :
	id(id),
	type(type)
{
	dataAccessor.crashIfUninitialized();
}

void BaseComponent::setDataStore(const DataStore& dataStore)
{
	dataAccessor.set(dataStore);
}

const DataStore& BaseComponent::getDataStore()
{
	return dataAccessor.getSafe();
}

const DataStore& BaseComponent::dataStore() const
{
	return dataAccessor.get();
}


ComponentIdType BaseComponent::getId() const
{
	return id;
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