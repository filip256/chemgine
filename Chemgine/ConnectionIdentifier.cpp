#include "ConnectionIdentifier.hpp"
#include "LabwareSystem.hpp"

PortIdentifier::PortIdentifier(
	LabwareSystem& system,
	const l_size componentIdx,
	const uint8_t portIdx
) noexcept :
	system(system),
	componentIdx(componentIdx),
	portIdx(portIdx)
{}

bool PortIdentifier::isValid() const
{
	return componentIdx != LabwareSystem::npos;
}

l_size PortIdentifier::getComponentIndex() const
{
	return componentIdx;
}

uint8_t PortIdentifier::getPortIndex() const
{
	return portIdx;
}

const BaseLabwareComponent& PortIdentifier::getComponent() const
{
	return *system.components[componentIdx];
}

BaseLabwareComponent& PortIdentifier::getComponent()
{
	return *system.components[componentIdx];
}

const LabwareSystem& PortIdentifier::getSystem() const
{
	return system;
}

LabwareSystem& PortIdentifier::getSystem()
{
	return system;
}

const DrawableLabwareConnection* PortIdentifier::operator->() const
{
	return &system.components[componentIdx]->getPort(portIdx);
}
