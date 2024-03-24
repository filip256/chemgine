#include "HeatsourceData.hpp"

HeatsourceData::HeatsourceData(
	const LabwareId id,
	const std::string& name,
	std::vector<LabwarePort>&& ports,
	const std::string& textureFile,
	const float textureScale,
	const Amount<Unit::WATT> maxPowerOutput
) noexcept :
	DrawableLabwareData(id, name, std::move(ports), textureFile, textureScale, LabwareType::HEATSOURCE),
	maxPowerOutput(maxPowerOutput)
{}
