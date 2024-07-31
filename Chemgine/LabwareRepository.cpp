#include "LabwareRepository.hpp"
#include "DataHelpers.hpp"
#include "LabwareDataFactory.hpp"
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
	const auto id = definition.pullProperty("id", DataHelpers::parseId<LabwareId>);
	const auto name = definition.pullDefaultProperty(Keywords::Labware::Name, "?");
	auto ports = definition.pullProperty(Keywords::Labware::Ports, DataHelpers::parseList<LabwarePort>, ',', true);
	const auto volume = definition.pullProperty(Keywords::Labware::Volume, DataHelpers::parseUnsigned<Unit::LITER>);
	const auto tx = definition.pullProperty(Keywords::Labware::Texture);
	const auto txScale = definition.pullDefaultProperty(Keywords::Labware::TextureScale, 1.0f, DataHelpers::parseUnsigned<float>);

	if (id.has_value() == false || volume.has_value() == false ||
		ports.has_value() == false || tx.has_value() == false)
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
	const auto id = definition.pullProperty("id", DataHelpers::parseId<LabwareId>);
	const auto name = definition.pullDefaultProperty(Keywords::Labware::Name, "?");
	auto ports = definition.pullProperty(Keywords::Labware::Ports, DataHelpers::parseList<LabwarePort>, ',', true);
	const auto volume = definition.pullProperty(Keywords::Labware::Volume, DataHelpers::parseUnsigned<Unit::LITER>);
	const auto tx = definition.pullProperty(Keywords::Labware::Texture);
	const auto txScale = definition.pullDefaultProperty(Keywords::Labware::TextureScale, 1.0f, DataHelpers::parseUnsigned<float>);

	if (id.has_value() == false || volume.has_value() == false ||
		ports.has_value() == false || tx.has_value() == false)
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
	const auto id = definition.pullProperty("id", DataHelpers::parseId<LabwareId>);
	const auto name = definition.pullDefaultProperty(Keywords::Labware::Name, "?");
	auto ports = definition.pullProperty(Keywords::Labware::Ports, DataHelpers::parseList<LabwarePort>, ',', true);
	const auto volume = definition.pullProperty(Keywords::Labware::Volume, DataHelpers::parseUnsigned<Unit::LITER>);
	const auto length = definition.pullProperty(Keywords::Labware::Length, DataHelpers::parseUnsigned<Unit::METER>);
	const auto effic = definition.pullProperty(Keywords::Labware::Effic, DataHelpers::parseUnsigned<Unit::PER_METER>);
	const auto tx = definition.pullProperty(Keywords::Labware::Texture);
	const auto inner = definition.pullProperty(Keywords::Labware::InnerMask);
	const auto coolant = definition.pullProperty(Keywords::Labware::CoolantMask);
	const auto txScale = definition.pullDefaultProperty(Keywords::Labware::TextureScale, 1.0f, DataHelpers::parseUnsigned<float>);

	if (id.has_value() == false || volume.has_value() == false ||
		length.has_value() == false || effic.has_value() == false ||
		ports.has_value() == false || tx.has_value() == false ||
		inner.has_value() == false || coolant.has_value() == false)
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
	const auto id = definition.pullProperty("id", DataHelpers::parseId<LabwareId>);
	const auto name = definition.pullDefaultProperty(Keywords::Labware::Name, "?");
	auto ports = definition.pullProperty(Keywords::Labware::Ports, DataHelpers::parseList<LabwarePort>, ',', true);
	const auto power = definition.pullProperty(Keywords::Labware::Power, DataHelpers::parseUnsigned<Unit::WATT>);
	const auto tx = definition.pullProperty(Keywords::Labware::Texture);
	const auto txScale = definition.pullDefaultProperty(Keywords::Labware::TextureScale, 1.0f, DataHelpers::parseUnsigned<float>);

	if (id.has_value() == false || power.has_value() == false ||
		ports.has_value() == false || tx.has_value() == false)
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

	const auto it = adders.find(definition.specifier);
	if (it != adders.end())
		return (this->*(it->second))(std::move(definition));

	Log(this).error("Unknown labware specifier: '{0}', at: {1}.", definition.specifier, definition.getLocationName());
	return false;
}

bool LabwareRepository::loadFromFile(const std::string& path)
{
	std::ifstream file(path);

	if (!file.is_open())
	{
		Log(this).error("Failed to open file '{0}'.", path);
		return false;
	}

	//if (files::verifyChecksum(file).code != 200) //not OK
	//	return StatusCode<>::FileCorrupted;

	table.clear();

	//parse file
	std::string buffer;
	std::getline(file, buffer);
	while (std::getline(file, buffer))
	{
		auto line = Utils::split(buffer, ',');

		if (line.size() < 3)
		{
			Log(this).error("Failed to load labware due to missing id or type.");
			continue;
		}

		const auto id = DataHelpers::parseId<LabwareId>(line[0]);
		const auto type = DataHelpers::parseEnum<LabwareType>(line[1]);

		if (id.has_value() == false || type.has_value() == false)
		{
			Log(this).error("Failed to load labware due to missing id or type.");
			continue;
		}

		const auto ptr = LabwareDataFactory::get(*id, *type, line);
		if(ptr == nullptr)
		{
			Log(this).error("Failed to load labware with id {0}.", *id);
			continue;
		}

		if (table.emplace(*id, ptr).second == false)
		{
			Log(this).warn("Duplicate labware with id {0} skipped.", *id);
		}
	}
	file.close();

	Log(this).info("Loaded {0} labware items.", table.size());

	return true;
}

const BaseLabwareData& LabwareRepository::at(const LabwareId id) const
{
	return *table.at(id);
}
