#pragma once

#include "ContainerLabwareData.hpp"

class FlaskData : public ContainerLabwareData
{
public:
	FlaskData(
		const LabwareIdType id,
		const std::string& name,
		std::vector<LabwarePort>&& ports,
		const Amount<Unit::LITER> volume,
		const std::string& textureFile
	) noexcept;
};