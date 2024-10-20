#include "HeatsourceData.hpp"

HeatsourceData::HeatsourceData(
	const LabwareId id,
	const std::string& name,
	std::vector<LabwarePort>&& ports,
	const Amount<Unit::WATT> maxPowerOutput,
	const std::string& textureFile,
	const float_n textureScale
) noexcept :
	DrawableLabwareData(id, name, std::move(ports), textureFile, textureScale, LabwareType::HEATSOURCE),
	maxPowerOutput(maxPowerOutput)
{}

void HeatsourceData::dumpCustomProperties(DataDumper& dump) const
{
	dump.propertyWithSep(Def::Labware::Power, maxPowerOutput)
		.propertyWithSep(Def::Labware::Texture, "~/" + textureFile)
		.property(Def::Labware::TextureScale, textureScale);
}
