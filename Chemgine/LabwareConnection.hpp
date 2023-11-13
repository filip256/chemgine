#pragma once

#include "SizeTypedefs.hpp"
#include "PortType.hpp"

#include <cstdint>

class LabwareConnection
{
public:
	const l_size other;
	const uint8_t otherPort, thisPort;
	const uint8_t sealLevel;

	LabwareConnection(
		const l_size other,
		const uint8_t otherPort,
		const uint8_t thisPort,
		const uint8_t sealLevel
	) noexcept;

	static uint8_t getSealLevel(const PortType source, const PortType destination);
};