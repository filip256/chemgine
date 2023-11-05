#pragma once

#include "DrawableLabwareData.hpp"
#include "LabwareJoint.hpp"
#include "Amount.hpp"

class ContainerLabwareData : public DrawableLabwareData
{
protected:
	ContainerLabwareData(
		const LabwareIdType id,
		const std::string& name,
		const std::string& textureFile,
		const Amount<Unit::LITER> volume,
		std::vector<LabwareJoint>&& joints,
		const LabwareType type
	) noexcept;

public:
	const Amount<Unit::LITER> volume;
	const std::vector<LabwareJoint> joints;
};