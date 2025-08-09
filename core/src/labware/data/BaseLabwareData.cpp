#include "labware/data/BaseLabwareData.hpp"

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
	static const auto valueOffset = checked_cast<uint8_t>(utils::max(
		def::Labware::Id.size(),
		def::Labware::Name.size(),
		def::Labware::Ports.size(),
		def::Labware::Volume.size(),
		def::Labware::Length.size(),
		def::Labware::Efficiency.size(),
		def::Labware::InnerMask.size(),
		def::Labware::CoolantMask.size(),
		def::Labware::Ports.size(),
		def::Labware::Texture.size(),
		def::Labware::TextureScale.size()));

	def::DataDumper dump(out, valueOffset, 0, prettify);
	dump.header(def::Types::Labware, type, "")
		.beginProperties()
		.propertyWithSep(def::Labware::Id, id)
		.propertyWithSep(def::Labware::Name, name)
		.propertyWithSep(def::Labware::Ports, ports);

	dumpCustomProperties(dump);
	dump.endProperties()
		.endDefinition();
}

void BaseLabwareData::print(std::ostream& out) const
{
	dumpDefinition(out, true);
}
