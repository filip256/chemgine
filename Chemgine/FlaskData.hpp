#pragma once

#include "ContainerLabwareData.hpp"

class FlaskData : public ContainerLabwareData
{
public:
	FlaskData(
		const LabwareId id,
		const std::string& name,
		std::vector<LabwarePort>&& ports,
		const Amount<Unit::LITER> volume,
		const std::string& textureFile
	) noexcept;
};