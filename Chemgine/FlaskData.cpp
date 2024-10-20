#include "FlaskData.hpp"

#include <iostream>

FlaskData::FlaskData(
	const LabwareId id,
	const std::string& name,
	std::vector<LabwarePort>&& ports,
	const Amount<Unit::LITER> volume,
	const std::string& textureFile,
	const float_n textureScale
) noexcept :
	ContainerLabwareData(id, name, std::move(ports), textureFile, textureScale, volume, LabwareType::FLASK)
{}

void FlaskData::dumpCustomProperties(DataDumper& dump) const
{
	dump.propertyWithSep(Def::Labware::Volume, getVolume())
		.propertyWithSep(Def::Labware::Texture, "~/" + textureFile)
		.property(Def::Labware::TextureScale, textureScale);
}
