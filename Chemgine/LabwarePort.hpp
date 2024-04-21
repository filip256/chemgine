#pragma once

#include "PortType.hpp"
#include "Amount.hpp"

class LabwarePort
{
public:
	const PortType type;
	const float x, y;
	const Amount<Unit::DEGREE> angle;

	LabwarePort(
		const PortType type,
		const float x, const float y,
		const Amount<Unit::DEGREE> angle
	) noexcept;
};
