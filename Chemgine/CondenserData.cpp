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
	const float_n textureScale
) noexcept :
	ContainerLabwareData<2>(
		id, name, std::move(ports), textureFile, textureScale,
		{ innerVolume, innerVolume }, { Ref(innerfillTextureFile), Ref(coolantfillTextureFile) },
		LabwareType::CONDENSER
	),
	length(length),
	efficiency(efficiency)
{}

void CondenserData::printDefinition(std::ostream& out) const
{
	out << '_' << Keywords::Types::Labware;
	out << ':' << Def::print(type);
	out << '{';
	out << Keywords::Labware::Id << ':' << Def::printId(id) << ',';
	out << Keywords::Labware::Name << ':' << name << ',';
	out << Keywords::Labware::Ports << ':' << Def::print(ports) << ',';
	out << Keywords::Labware::Volume << ':' << Def::print(getVolume<0>()) << ',';
	out << Keywords::Labware::Length << ':' << Def::print(length) << ',';
	out << Keywords::Labware::Efficiency << ':' << Def::print(efficiency) << ',';
	out << Keywords::Labware::Texture << ':' << "~/" << textureFile << ',';
	out << Keywords::Labware::InnerMask << ':' << "~/" << "fill0_" << textureFile << ',';
	out << Keywords::Labware::CoolantMask << ':' << "~/" << "fill1_" << textureFile << ',';
	out << Keywords::Labware::TextureScale << ':' << textureScale;
	out << "};\n";
}
