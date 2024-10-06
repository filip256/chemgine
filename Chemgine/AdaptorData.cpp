#include "AdaptorData.hpp"
#include "PathUtils.hpp"

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

void AdaptorData::printDefinition(std::ostream& out) const
{
	out << '_' << Keywords::Types::Labware;
	out << ':' << Def::print(type);
	out << '{';
	out << Keywords::Labware::Id << ':' << Def::printId(id) << ',';
	out << Keywords::Labware::Name << ':' << name << ',';
	out << Keywords::Labware::Ports << ':' << Def::print(ports) << ',';
	out << Keywords::Labware::Volume << ':' << Def::print(getVolume()) << ',';
	out << Keywords::Labware::Texture << ':' << "~/" << textureFile << ',';
	out << Keywords::Labware::TextureScale << ':' << textureScale;
	out << "};\n";
}
