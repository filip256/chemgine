#include "LabwareRepository.hpp"
#include "Parsers.hpp"
#include "FlaskData.hpp"
#include "AdaptorData.hpp"
#include "CondenserData.hpp"
#include "HeatsourceData.hpp"
#include "Keywords.hpp"
#include "Log.hpp"

bool LabwareRepository::checkTextureFile(std::string path, const Def::Location& location)
{
	if (not Utils::fileExists(path))
	{
		Log(this).error("Unable to find texture file: '{0}', at: {1}.", path, location.toString());
		return false;
	}
	if (const auto ext = Utils::extractExtension(path); ext != "png")
	{
		Log(this).error("Invalid texture file extension: '{0}', at: {1}.", ext, location.toString());
		return false;
	}

	return true;
}

template <>
bool LabwareRepository::add<LabwareType::FLASK>(const LabwareId id, const Def::Object& definition)
{
	const auto name = definition.getDefaultProperty(Def::Labware::Name, "?");
	auto ports = definition.getProperty(Def::Labware::Ports, Def::parse<std::vector<LabwarePort>>);
	const auto volume = definition.getProperty(Def::Labware::Volume, Def::parse<Amount<Unit::LITER>>);
	const auto tx = definition.getProperty(Def::Labware::Texture);
	const auto txScale = definition.getDefaultProperty(Def::Labware::TextureScale, 1.0f, Def::parseUnsigned<float_s>);

	if (not(volume && ports && tx))
	{
		Log(this).error("Incomplete flask definition at: {0}.", definition.getLocationName());
		return false;
	}

	if (not checkTextureFile(*tx, definition.getLocation()))
		return false;

	labware.emplace(id,
		std::make_unique<FlaskData>(id, name, std::move(*ports), *volume, *tx, txScale));
	return true;
}

template <>
bool LabwareRepository::add<LabwareType::ADAPTOR>(const LabwareId id, const Def::Object& definition)
{
	const auto name = definition.getDefaultProperty(Def::Labware::Name, "?");
	auto ports = definition.getProperty(Def::Labware::Ports, Def::parse<std::vector<LabwarePort>>);
	const auto volume = definition.getProperty(Def::Labware::Volume, Def::parse<Amount<Unit::LITER>>);
	const auto tx = definition.getProperty(Def::Labware::Texture);
	const auto txScale = definition.getDefaultProperty(Def::Labware::TextureScale, 1.0f, Def::parseUnsigned<float_s>);

	if (not(volume && ports && tx))
	{
		Log(this).error("Incomplete adaptor definition at: {0}.", definition.getLocationName());
		return false;
	}

	if (not checkTextureFile(*tx, definition.getLocation()))
		return false;

	labware.emplace(id,
		std::make_unique<AdaptorData>(id, name, std::move(*ports), *volume, *tx, txScale));
	return true;
}

template <>
bool LabwareRepository::add<LabwareType::CONDENSER>(const LabwareId id, const Def::Object& definition)
{
	const auto name = definition.getDefaultProperty(Def::Labware::Name, "?");
	auto ports = definition.getProperty(Def::Labware::Ports, Def::parse<std::vector<LabwarePort>>);
	const auto volume = definition.getProperty(Def::Labware::Volume, Def::parse<Amount<Unit::LITER>>);
	const auto length = definition.getProperty(Def::Labware::Length, Def::parse<Amount<Unit::METER>>);
	const auto effic = definition.getProperty(Def::Labware::Efficiency, Def::parse<Amount<Unit::PER_METER>>);
	const auto tx = definition.getProperty(Def::Labware::Texture);
	const auto inner = definition.getProperty(Def::Labware::InnerMask);
	const auto coolant = definition.getProperty(Def::Labware::CoolantMask);
	const auto txScale = definition.getDefaultProperty(Def::Labware::TextureScale, 1.0f, Def::parseUnsigned<float_s>);

	if (not(volume && length && effic && ports && tx && inner && coolant))
	{
		Log(this).error("Incomplete condenser definition at: {0}.", definition.getLocationName());
		return false;
	}

	if (not (
		checkTextureFile(*tx, definition.getLocation()) &&
		checkTextureFile(*inner, definition.getLocation()) &&
		checkTextureFile(*coolant, definition.getLocation())))
		return false;

	labware.emplace(id,
		std::make_unique<CondenserData>(id, name, std::move(*ports), *volume, *length, *effic, *tx, *inner, *coolant, txScale));
	return true;
}

template <>
bool LabwareRepository::add<LabwareType::HEATSOURCE>(const LabwareId id, const Def::Object& definition)
{
	const auto name = definition.getDefaultProperty(Def::Labware::Name, "?");
	auto ports = definition.getProperty(Def::Labware::Ports, Def::parse<std::vector<LabwarePort>>);
	const auto power = definition.getProperty(Def::Labware::Power, Def::parse<Amount<Unit::WATT>>);
	const auto tx = definition.getProperty(Def::Labware::Texture);
	const auto txScale = definition.getDefaultProperty(Def::Labware::TextureScale, 1.0f, Def::parseUnsigned<float_s>);

	if (not(power && ports && tx))
	{
		Log(this).error("Incomplete heatsource definition at: {0}.", definition.getLocationName());
		return false;
	}

	if (not checkTextureFile(*tx, definition.getLocation()))
		return false;

	labware.emplace(id,
		std::make_unique<HeatsourceData>(id, name, std::move(*ports), *power, *tx, txScale));
	return true;
}

bool LabwareRepository::add(const Def::Object& definition)
{
	static std::unordered_map<std::string, bool (LabwareRepository::*)(LabwareId, const Def::Object&)> adders =
	{
		{Def::Labware::Flask, &LabwareRepository::add<LabwareType::FLASK> },
		{Def::Labware::Adaptor, &LabwareRepository::add<LabwareType::ADAPTOR> },
		{Def::Labware::Condenser, &LabwareRepository::add<LabwareType::CONDENSER> },
		{Def::Labware::Heatsource, &LabwareRepository::add<LabwareType::HEATSOURCE> }
	};

	const auto it = adders.find(definition.getSpecifier());
	if (it == adders.end())
	{
		Log(this).error("Unknown labware specifier: '{0}', at: {1}.", definition.getSpecifier(), definition.getLocationName());
		return false;
	}

	const auto id = definition.getProperty(Def::Labware::Id, Def::parse<LabwareId>);
	if (not id)
		return false;

	if (labware.contains(*id))
	{
		Log(this).error("Labware with duplicate id: '{0}', at: {1}.", *id, definition.getLocationName());
		return false;
	}

	const auto success = (this->*(it->second))(*id, definition);
	if (not success)
		return false;

	definition.logUnusedWarnings();
	return true;
}

bool LabwareRepository::contains(const LabwareId id) const
{
	return labware.contains(id);
}

const BaseLabwareData& LabwareRepository::at(const LabwareId id) const
{
	return *labware.at(id);
}

size_t LabwareRepository::totalDefinitionCount() const
{
	return labware.size();
}

LabwareRepository::Iterator LabwareRepository::begin() const
{
	return labware.begin();
}

LabwareRepository::Iterator LabwareRepository::end() const
{
	return labware.end();
}

size_t LabwareRepository::size() const
{
	return labware.size();
}

void LabwareRepository::clear()
{
	labware.clear();
}
