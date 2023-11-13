#include "ContainerLabwareData.hpp"

ContainerLabwareData::ContainerLabwareData(
	const LabwareIdType id,
	const std::string& name,
	std::vector<LabwarePort>&& ports,
	const std::string& textureFile,
	const Amount<Unit::LITER> volume,
	const LabwareType type
) noexcept :
	DrawableLabwareData(id, name, std::move(ports), textureFile, type),
	volume(volume)
{}