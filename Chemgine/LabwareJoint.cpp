#include "LabwareJoint.hpp"

LabwareJoint::LabwareJoint(
	const PortType type,
	const float x,
	const float y,
	const float angle
) noexcept :
	type(type),
	x(x),
	y(y),
	angle(angle)
{}