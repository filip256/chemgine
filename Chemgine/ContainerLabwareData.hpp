#pragma once

#include "DrawableLabwareData.hpp"
#include "Amount.hpp"

class ContainerLabwareData : public DrawableLabwareData
{
protected:
	ContainerLabwareData(
		const LabwareIdType id,
		const std::string& name,
		std::vector<LabwarePort>&& ports,
		const std::string& textureFile,
		const Amount<Unit::LITER> volume,
		const LabwareType type
	) noexcept;

public:
	const Amount<Unit::LITER> volume;
};