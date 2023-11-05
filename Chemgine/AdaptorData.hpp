#pragma once

#include "ContainerLabwareData.hpp"

class AdaptorData : public ContainerLabwareData
{
public:
	AdaptorData(
		const LabwareIdType id,
		const std::string& name,
		const Amount<Unit::LITER> volume,
		std::vector<LabwareJoint>&& joints,
		const std::string& textureFile
	) noexcept;
};