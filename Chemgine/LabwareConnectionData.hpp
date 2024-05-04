#pragma once

#include "PortType.hpp"
#include "Amount.hpp"

class LabwareConnectionData
{
protected:
	LabwareConnectionData(
		const PortType type,
		const float x, const float y,
		const Amount<Unit::DEGREE> angle
	) noexcept;

public:
	const PortType type;
	const float x, y;
	const Amount<Unit::DEGREE> angle;

};

class LabwarePortData : public LabwareConnectionData
{
public:
	LabwarePortData(
		const PortType type,
		const float x, const float y,
		const Amount<Unit::DEGREE> angle
	) noexcept;
};

class LabwareContactData : public LabwareConnectionData
{
public:
	LabwareContactData(
		const PortType type,
		const float x, const float y,
		const Amount<Unit::DEGREE> angle
	) noexcept;
};
