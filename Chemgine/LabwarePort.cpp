#include "LabwarePort.hpp"

LabwarePort::LabwarePort(
	const PortType type,
	const float x, const float y,
	const Amount<Unit::DEGREE> angle
) noexcept :
	type(type),
	x(x),
	y(y),
	angle(angle)
{}
