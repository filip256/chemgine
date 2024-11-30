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
	const float_s textureScale
) noexcept :
	ContainerLabwareData<2>(
		id, name, std::move(ports), textureFile, textureScale,
		{ innerVolume, innerVolume }, { Ref(innerfillTextureFile), Ref(coolantfillTextureFile) },
		LabwareType::CONDENSER
	),
	length(length),
	efficiency(efficiency)
{}

void CondenserData::dumpCustomProperties(Def::DataDumper& dump) const
{
	dump.propertyWithSep(Def::Labware::Volume, getVolume<0>())
		.propertyWithSep(Def::Labware::Length, length)
		.propertyWithSep(Def::Labware::Efficiency, efficiency)
		.propertyWithSep(Def::Labware::Texture, "~/" + textureFile)
		.propertyWithSep(Def::Labware::InnerMask, "~/fill0_" + textureFile)
		.propertyWithSep(Def::Labware::CoolantMask, "~/fill1_" + textureFile)
		.property(Def::Labware::TextureScale, textureScale);
}
