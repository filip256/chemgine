#include "LabwarePort.hpp"

LabwarePort::LabwarePort(
	const PortType type,
	const float_s x, const float_s y,
	const Amount<Unit::DEGREE> angle
) noexcept :
	type(type),
	x(x),
	y(y),
	angle(angle)
{}
