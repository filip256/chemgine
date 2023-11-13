#pragma once

#include "ContainerLabwareData.hpp"

class AdaptorData : public ContainerLabwareData
{
public:
	AdaptorData(
		const LabwareIdType id,
		const std::string& name,
		std::vector<LabwarePort>&& ports,
		const Amount<Unit::LITER> volume,
		const std::string& textureFile
	) noexcept;
};