#include "LabwareConnectionData.hpp"

LabwareConnectionData::LabwareConnectionData(
	const PortType type,
	const float x, const float y,
	const Amount<Unit::DEGREE> angle
) noexcept :
	type(type),
	x(x),
	y(y),
	angle(angle)
{}

LabwarePortData::LabwarePortData(
	const PortType type,
	const float x, const float y,
	const Amount<Unit::DEGREE> angle
) noexcept :
	LabwareConnectionData(type, x, y, angle)
{}

LabwareContactData::LabwareContactData(
	const PortType type,
	const float x, const float y,
	const Amount<Unit::DEGREE> angle
) noexcept :
	LabwareConnectionData(type, x, y, angle)
{}
