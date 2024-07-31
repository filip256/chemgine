#include "CondenserData.hpp"

CondenserData::CondenserData(
	const LabwareId id,
	const std::string& name,
	std::vector<LabwarePort>&& ports,
	const Amount<Unit::LITER> innerVolume,
	const Amount<Unit::METER> length,
	const Amount<Unit::PER_METER> efficiency,
	const std::string& textureFile,
	const std::string& innerfillTextureFile,
	const std::string& coolantfillTextureFile,
	const float textureScale
) noexcept :
	ContainerLabwareData<2>(
		id, name, std::move(ports), textureFile, textureScale,
		{ innerVolume, innerVolume }, { Ref(innerfillTextureFile), Ref(coolantfillTextureFile) },
		LabwareType::CONDENSER
	),
	length(length),
	efficiency(efficiency)
{}
