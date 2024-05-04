#include "Flask.hpp"
#include "DataStore.hpp"
#include "ConnectionIdentifier.hpp"

Flask::Flask(
	const LabwareId id,
	Atmosphere& atmosphere
) noexcept :
	ContainerComponent(id, LabwareType::FLASK, atmosphere)
{}

const FlaskData& Flask::getData() const
{
	return static_cast<const FlaskData&>(data);
}

bool Flask::tryConnect(PortIdentifier& thisPort, PortIdentifier& otherPort)
{
	auto& otherComp = otherPort.getComponent();
	const auto otherPortId = otherPort.getPortIndex();

	if (otherComp.isContainer())
	{
		this->setOverflowTarget(otherComp.as<BaseContainerComponent&>(), otherPortId);
		return true;
	}

	return false;
}

void Flask::disconnect(PortIdentifier& thisPort, PortIdentifier& otherPort, const Ref<BaseContainer> dump)
{
	const auto otherPortId = otherPort.getPortIndex();
	this->setOverflowTarget(dump, otherPortId);
}
