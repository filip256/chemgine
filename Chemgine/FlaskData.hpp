#pragma once

#include "ContainerLabwareData.hpp"

class FlaskData : public ContainerLabwareData<1>
{
public:
	FlaskData(
		const LabwareId id,
		const std::string& name,
		std::vector<LabwarePortData>&& ports,
		std::vector<LabwareContactData>&& contacts,
		const Amount<Unit::LITER> volume,
		const std::string& textureFile,
		const float textureScale
	) noexcept;
};
