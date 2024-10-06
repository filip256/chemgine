#include "HeatsourceData.hpp"

HeatsourceData::HeatsourceData(
	const LabwareId id,
	const std::string& name,
	std::vector<LabwarePort>&& ports,
	const Amount<Unit::WATT> maxPowerOutput,
	const std::string& textureFile,
	const float textureScale
) noexcept :
	DrawableLabwareData(id, name, std::move(ports), textureFile, textureScale, LabwareType::HEATSOURCE),
	maxPowerOutput(maxPowerOutput)
{}

void HeatsourceData::printDefinition(std::ostream& out) const
{
	out << '_' << Keywords::Types::Labware;
	out << ':' << Def::print(type);
	out << '{';
	out << Keywords::Labware::Id << ':' << Def::printId(id) << ',';
	out << Keywords::Labware::Name << ':' << name << ',';
	out << Keywords::Labware::Ports << ':' << Def::print(ports) << ',';
	out << Keywords::Labware::Power << ':' << Def::print(maxPowerOutput) << ',';
	out << Keywords::Labware::Texture << ':' << "~/" << textureFile << ',';
	out << Keywords::Labware::TextureScale << ':' << textureScale;
	out << "};\n";
}
