#include "LabwareConnection.hpp"

LabwareConnection::LabwareConnection(
	const l_size other,
	const uint8_t otherPort,
	const uint8_t thisPort,
	const uint8_t sealLevel
) noexcept :
	other(other),
	otherPort(otherPort),
	thisPort(thisPort),
	sealLevel(sealLevel)
{}

uint8_t LabwareConnection::getSealLevel(const PortType source, const PortType destination)
{
	if (source == PortType::INLET)
	{
		if (destination == PortType::OUTLET)
			return 250;

		if (destination == PortType::INLET)
			return 0;
	}

	if (destination == PortType::INLET)
	{
		if (source == PortType::OUTLET)
			return 250;

		if (source == PortType::INLET)
			return 0;
	}

	if (source == PortType::HOSE || destination == PortType::HOSE)
	{
		if (source != destination)
			return 0;
		return 255;
	}

	if (source == PortType::SIMPLE)
	{
		if (destination == PortType::SIMPLE)
			return 0;
		return 1;
	}

	if (destination == PortType::SIMPLE)
		return 220;

	if (source == PortType::DROPPER)
		return 220;

	if (destination == PortType::DROPPER)
		return 0;

	return 0;
}