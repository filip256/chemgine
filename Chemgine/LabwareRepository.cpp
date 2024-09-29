#include "LabwareRepository.hpp"
#include "Parsers.hpp"
#include "FlaskData.hpp"
#include "AdaptorData.hpp"
#include "CondenserData.hpp"
#include "HeatsourceData.hpp"
#include "Keywords.hpp"
#include "Log.hpp"

#include <fstream>

LabwareRepository::~LabwareRepository() noexcept
{
	for (const auto& l : table)
		delete l.second;
}

template <>
bool LabwareRepository::add<LabwareType::FLASK>(DefinitionObject&& definition)
{
	const auto id = definition.pullProperty("id", Def::parseId<LabwareId>);
	const auto name = definition.pullDefaultProperty(Keywords::Labware::Name, "?");
	auto ports = definition.pullProperty(Keywords::Labware::Ports, Def::parse<std::vector<LabwarePort>>);
	const auto volume = definition.pullProperty(Keywords::Labware::Volume, Def::parse<Amount<Unit::LITER>>);
	const auto tx = definition.pullProperty(Keywords::Labware::Texture);
	const auto txScale = definition.pullDefaultProperty(Keywords::Labware::TextureScale, 1.0f, Def::parseUnsigned<float>);

	if (not(id && volume && ports && tx))
	{
		Log(this).error("Incomplete flask definition at: {0}.", definition.getLocationName());
		return false;
	}

	const auto flask = new FlaskData(*id, name, std::move(*ports), *volume, *tx, txScale);
	if (table.emplace(*id, flask).second == false)
	{
		Log(this).warn("Duplicate labware with id: '{0}' skipped, at: {1}.", *id, definition.getLocationName());
		return false;
	}

	return true;
}

template <>
bool LabwareRepository::add<LabwareType::ADAPTOR>(DefinitionObject&& definition)
{
	const auto id = definition.pullProperty("id", Def::parseId<LabwareId>);
	const auto name = definition.pullDefaultProperty(Keywords::Labware::Name, "?");
	auto ports = definition.pullProperty(Keywords::Labware::Ports, Def::parse<std::vector<LabwarePort>>);
	const auto volume = definition.pullProperty(Keywords::Labware::Volume, Def::parse<Amount<Unit::LITER>>);
	const auto tx = definition.pullProperty(Keywords::Labware::Texture);
	const auto txScale = definition.pullDefaultProperty(Keywords::Labware::TextureScale, 1.0f, Def::parseUnsigned<float>);

	if (not(id && volume && ports && tx))
	{
		Log(this).error("Incomplete adaptor definition at: {0}.", definition.getLocationName());
		return false;
	}

	const auto adaptor = new AdaptorData(*id, name, std::move(*ports), *volume, *tx, txScale);
	if (table.emplace(*id, adaptor).second == false)
	{
		Log(this).warn("Duplicate labware with id: '{0}' skipped, at: {1}.", *id, definition.getLocationName());
		return false;
	}

	return true;
}

template <>
bool LabwareRepository::add<LabwareType::CONDENSER>(DefinitionObject&& definition)
{
	const auto id = definition.pullProperty("id", Def::parseId<LabwareId>);
	const auto name = definition.pullDefaultProperty(Keywords::Labware::Name, "?");
	auto ports = definition.pullProperty(Keywords::Labware::Ports, Def::parse<std::vector<LabwarePort>>);
	const auto volume = definition.pullProperty(Keywords::Labware::Volume, Def::parse<Amount<Unit::LITER>>);
	const auto length = definition.pullProperty(Keywords::Labware::Length, Def::parse<Amount<Unit::METER>>);
	const auto effic = definition.pullProperty(Keywords::Labware::Effic, Def::parse<Amount<Unit::PER_METER>>);
	const auto tx = definition.pullProperty(Keywords::Labware::Texture);
	const auto inner = definition.pullProperty(Keywords::Labware::InnerMask);
	const auto coolant = definition.pullProperty(Keywords::Labware::CoolantMask);
	const auto txScale = definition.pullDefaultProperty(Keywords::Labware::TextureScale, 1.0f, Def::parseUnsigned<float>);

	if (not(id && volume && length && effic && ports && tx && inner && coolant))
	{
		Log(this).error("Incomplete condenser definition at: {0}.", definition.getLocationName());
		return false;
	}

	const auto condenser = new CondenserData(*id, name, std::move(*ports), *volume, *length, *effic, *tx, *inner, *coolant, txScale);
	if (table.emplace(*id, condenser).second == false)
	{
		Log(this).warn("Duplicate labware with id: '{0}' skipped, at: {1}.", *id, definition.getLocationName());
		return false;
	}

	return true;
}

template <>
bool LabwareRepository::add<LabwareType::HEATSOURCE>(DefinitionObject&& definition)
{
	const auto id = definition.pullProperty("id", Def::parseId<LabwareId>);
	const auto name = definition.pullDefaultProperty(Keywords::Labware::Name, "?");
	auto ports = definition.pullProperty(Keywords::Labware::Ports, Def::parse<std::vector<LabwarePort>>);
	const auto power = definition.pullProperty(Keywords::Labware::Power, Def::parse<Amount<Unit::WATT>>);
	const auto tx = definition.pullProperty(Keywords::Labware::Texture);
	const auto txScale = definition.pullDefaultProperty(Keywords::Labware::TextureScale, 1.0f, Def::parseUnsigned<float>);

	if (not(id && power && ports && tx))
	{
		Log(this).error("Incomplete heatsource definition at: {0}.", definition.getLocationName());
		return false;
	}

	const auto& ignored = definition.getRemainingProperties();
	for (const auto& [name, _] : ignored)
		Log(this).warn("Ignored unknown labware property: '{0}', at: {1}.", name, definition.getLocationName());

	const auto heatsource = new HeatsourceData(*id, name, std::move(*ports), *power, *tx, txScale);
	if (table.emplace(*id, heatsource).second == false)
	{
		Log(this).warn("Duplicate labware with id: '{0}' skipped, at: {1}.", *id, definition.getLocationName());
		return false;
	}

	return true;
}

bool LabwareRepository::add(DefinitionObject&& definition)
{
	static std::unordered_map<std::string, bool (LabwareRepository::*)(DefinitionObject&&)> adders =
	{
		{Keywords::Labware::Flask, &LabwareRepository::add<LabwareType::FLASK> },
		{Keywords::Labware::Adaptor, &LabwareRepository::add<LabwareType::ADAPTOR> },
		{Keywords::Labware::Condenser, &LabwareRepository::add<LabwareType::CONDENSER> },
		{Keywords::Labware::Heatsource, &LabwareRepository::add<LabwareType::HEATSOURCE> }
	};

	const auto it = adders.find(definition.getSpecifier());
	if (it != adders.end())
		return (this->*(it->second))(std::move(definition));

	Log(this).error("Unknown labware specifier: '{0}', at: {1}.", definition.getSpecifier(), definition.getLocationName());
	return false;
}

const BaseLabwareData& LabwareRepository::at(const LabwareId id) const
{
	return *table.at(id);
}

void LabwareRepository::clear()
{
	table.clear();
}
