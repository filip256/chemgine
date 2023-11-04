#pragma once

#include "PortType.hpp"

class LabwareJoint
{
public:
	const PortType type;
	const float x, y;
	const float angle;

	LabwareJoint(const PortType type, const float x, const float y, const float angle) noexcept;
};