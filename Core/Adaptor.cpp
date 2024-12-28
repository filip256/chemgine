#include "Adaptor.hpp"
#include "DataStore.hpp"
#include "Flask.hpp"
#include "Condenser.hpp"

Adaptor::Adaptor(
	const LabwareId id,
	AtmosphereMixture& atmosphere
) noexcept :
	ContainerComponent(id, LabwareType::ADAPTOR, atmosphere)
{}

const AdaptorData& Adaptor::getData() const
{
	return static_cast<const AdaptorData&>(data);
}

bool Adaptor::tryConnect(LabwareComponentBase& other)
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
		if(id != 302)
			container.setOverflowTarget(other.as<Condenser&>().getContent());
		return true;
	}

	return false;
}

void Adaptor::disconnect(const Ref<ContainerBase> dump, const LabwareComponentBase& other)
{
	auto& container = getContent<0>();
	if (other.isFlask())
		container.setAllIncompatibilityTargets(dump);

	if (other.isAdaptor())
		container.setOverflowTarget(dump);
}
