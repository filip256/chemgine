#include "BaseLabwareComponent.hpp"
#include "DataStore.hpp"
#include "Log.hpp"

size_t BaseLabwareComponent::instanceCount = 0;

BaseLabwareComponent::BaseLabwareComponent(
	const LabwareId id,
	const LabwareType type
) noexcept :
	id(id),
	data(dataAccessor.getSafe().labware.at(id))
{
	if (type != data.type)
		Log(this).fatal("Labware component given by id {0} does not match the requested component type.", id);
}

bool BaseLabwareComponent::isFlask() const
{
	return data.type == LabwareType::FLASK;
}

bool BaseLabwareComponent::isAdaptor() const
{
	return data.type == LabwareType::ADAPTOR;
}

bool BaseLabwareComponent::isCondenser() const
{
	return data.type == LabwareType::CONDENSER;
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
