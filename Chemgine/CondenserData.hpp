#pragma once

#include "ContainerLabwareData.hpp"

class CondenserData : public ContainerLabwareData<2>
{
public:
	const Amount<Unit::METER> length;
	const Amount<Unit::PER_METER> efficiency;

	CondenserData(
		const LabwareId id,
		const std::string& name,
		std::vector<LabwarePortData>&& ports,
		const Amount<Unit::METER> length,
		const Amount<Unit::PER_METER> efficiency,
		const Amount<Unit::LITER> innerVolume,
		const std::string& textureFile,
		const float textureScale,
		const std::string& innerfillTextureFile,
		const std::string& coolantfillTextureFile
	) noexcept;
};
