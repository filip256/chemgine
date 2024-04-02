#include "BaseComponent.hpp"
#include "DataStore.hpp"
#include "Logger.hpp"

size_t BaseComponent::instanceCount = 0;
DataStoreAccessor BaseComponent::dataAccessor = DataStoreAccessor();

BaseComponent::BaseComponent(const ComponentId id, const ComponentType type) noexcept :
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


ComponentId BaseComponent::getId() const
{
	return id;
}

std::string BaseComponent::getSMILES() const
{
	return data().getSMILES();
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
	return component.type == ComponentType::COMPOSITE;
}

bool BaseComponent::isRadicalType() const
{
	return false;
}

bool BaseComponent::areEqual(const BaseComponent& x, const BaseComponent& y)
{
	return x.getId() == y.getId();
}

bool BaseComponent::areMatching(const BaseComponent& x, const BaseComponent& y)
{
	if (areEqual(x, y))
		return true;

	if (x.getId() == 101 || y.getId() == 101)
		return true;

	return false;
}



#ifndef NDEBUG
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
#endif