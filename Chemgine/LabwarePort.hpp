#pragma once

#include "PortType.hpp"

class LabwarePort
{
public:
	const PortType type;
	const float x, y;
	const float angle;

	LabwarePort(const PortType type, const float x, const float y, const float angle) noexcept;
};