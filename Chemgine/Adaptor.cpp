#include "Adaptor.hpp"
#include "DataStore.hpp"
#include "Flask.hpp"
#include "Condenser.hpp"

Adaptor::Adaptor(
	const LabwareId id,
	Atmosphere& atmosphere
) noexcept :
	ContainerComponent(id, atmosphere)
{}

const AdaptorData& Adaptor::getData() const
{
	return static_cast<const AdaptorData&>(data);
}

bool Adaptor::tryConnect(BaseLabwareComponent& other)
{
	auto& container = getContent<0>();
	if (other.isFlask())
	{
		container.setAllIncompatibilityTargets(other.as<Flask&>().getContent());
		return true;
	}

	if (other.isAdaptor())
	{
		container.setOverflowTarget(other.as<Adaptor&>().getContent());
		return true;
	}

	if (other.isCondenser())
	{
		container.setOverflowTarget(other.as<Condenser&>().getContent());
		return true;
	}

	return false;
}

void Adaptor::disconnect(const Ref<BaseContainer> dump, const BaseLabwareComponent& other)
{
	auto& container = getContent<0>();
	if (other.isFlask())
		container.setAllIncompatibilityTargets(dump);

	if (other.isAdaptor())
		container.setOverflowTarget(dump);
}
