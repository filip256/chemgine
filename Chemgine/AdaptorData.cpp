#include "AdaptorData.hpp"

AdaptorData::AdaptorData(
	const LabwareId id,
	const std::string& name,
	std::vector<LabwarePort>&& ports,
	const Amount<Unit::LITER> volume,
	const std::string& textureFile,
	const float textureScale
) noexcept :
	ContainerLabwareData(id, name, std::move(ports), textureFile, textureScale, volume, LabwareType::ADAPTOR)
{}
