#include "CondenserData.hpp"

CondenserData::CondenserData(
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
) noexcept :
	ContainerLabwareData<2>(
		id, name,
		std::move(ports), std::vector<LabwareContactData>(),
		textureFile, textureScale,
		{ innerVolume, innerVolume }, { Ref(innerfillTextureFile), Ref(coolantfillTextureFile) },
		LabwareType::CONDENSER
	),
	length(length),
	efficiency(efficiency)
{}
