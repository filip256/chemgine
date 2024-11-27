#include "BaseLabwareData.hpp"

BaseLabwareData::BaseLabwareData(
	const LabwareId id,
	const std::string& name,
	std::vector<LabwarePort>&& ports,
	const LabwareType type
) noexcept :
	id(id),
	name(name),
	ports(std::move(ports)),
	type(type)
{}

void BaseLabwareData::dumpDefinition(std::ostream& out, const bool prettify) const
{
	static const auto valueOffset = checked_cast<uint8_t>(Utils::max(
		Def::Labware::Id.size(),
		Def::Labware::Name.size(),
		Def::Labware::Ports.size(),
		Def::Labware::Volume.size(),
		Def::Labware::Length.size(),
		Def::Labware::Efficiency.size(),
		Def::Labware::InnerMask.size(),
		Def::Labware::CoolantMask.size(),
		Def::Labware::Ports.size(),
		Def::Labware::Texture.size(),
		Def::Labware::TextureScale.size()));

	Def::DataDumper dump(out, valueOffset, 0, prettify);
	dump.header(Def::Types::Labware, type, "")
		.beginProperties()
		.propertyWithSep(Def::Labware::Id, id)
		.propertyWithSep(Def::Labware::Name, name)
		.propertyWithSep(Def::Labware::Ports, ports);

	dumpCustomProperties(dump);
	dump.endProperties()
		.endDefinition();
}

void BaseLabwareData::print(std::ostream& out) const
{
	dumpDefinition(out, true);
}
