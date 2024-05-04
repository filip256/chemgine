#include "Adaptor.hpp"
#include "DataStore.hpp"
#include "Flask.hpp"
#include "Condenser.hpp"
#include "ConnectionIdentifier.hpp"

Adaptor::Adaptor(
	const LabwareId id,
	Atmosphere& atmosphere
) noexcept :
	ContainerComponent(id, LabwareType::ADAPTOR, atmosphere)
{}

const AdaptorData& Adaptor::getData() const
{
	return static_cast<const AdaptorData&>(data);
}

bool Adaptor::tryConnect(PortIdentifier& thisPort, PortIdentifier& otherPort)
{
	auto& container = getContent<0>();
	auto& otherComp = otherPort.getComponent();
	const auto otherPortId = otherPort.getPortIndex();

	if (otherComp.isFlask())
	{
		container.setAllIncompatibilityTargets(otherComp.as<Flask&>().getContent());
		return true;
	}

	if (otherComp.isAdaptor())
	{
		container.setOverflowTarget(otherComp.as<Adaptor&>().getContent(), otherPortId);
		return true;
	}

	if (otherComp.isCondenser())
	{
		container.setOverflowTarget(otherComp.as<Condenser&>().getContent(), otherPortId);
		return true;
	}

	return false;
}

void Adaptor::disconnect(PortIdentifier& thisPort, PortIdentifier& otherPort, const Ref<BaseContainer> dump)
{
	auto& container = getContent<0>();
	auto& otherComp = otherPort.getComponent();
	const auto otherPortId = otherPort.getPortIndex();

	if (otherComp.isFlask())
		container.setAllIncompatibilityTargets(dump);

	if (otherComp.isAdaptor())
		container.setOverflowTarget(dump, otherPortId);
}
