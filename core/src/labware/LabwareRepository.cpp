#include "labware/LabwareRepository.hpp"

#include "data/def/Keywords.hpp"
#include "data/def/Parsers.hpp"
#include "io/Log.hpp"
#include "labware/data/AdaptorData.hpp"
#include "labware/data/CondenserData.hpp"
#include "labware/data/FlaskData.hpp"
#include "labware/data/HeatsourceData.hpp"

bool LabwareRepository::checkTextureFile(std::string path, const def::Location& location)
{
    if (not utils::fileExists(path)) {
        Log(this).error("Unable to find texture file: '{0}', at: {1}.", path, location.toString());
        return false;
    }
    if (const auto ext = utils::extractExtension(path); ext != "png") {
        Log(this).error("Invalid texture file extension: '{0}', at: {1}.", ext, location.toString());
        return false;
    }

    return true;
}

template <>
bool LabwareRepository::add<LabwareType::FLASK>(const LabwareId id, const def::Object& definition)
{
    const auto name    = definition.getDefaultProperty(def::Labware::Name, "?");
    auto       ports   = definition.getProperty(def::Labware::Ports, def::parse<std::vector<LabwarePort>>);
    const auto volume  = definition.getProperty(def::Labware::Volume, def::parse<Amount<Unit::LITER>>);
    const auto tx      = definition.getProperty(def::Labware::Texture);
    const auto txScale = definition.getDefaultProperty(def::Labware::TextureScale, 1.0f, def::parseUnsigned<float_s>);

    if (not(volume && ports && tx)) {
        Log(this).error("Incomplete flask definition at: {0}.", definition.getLocationName());
        return false;
    }

    if (not checkTextureFile(*tx, definition.getLocation()))
        return false;

    labware.emplace(id, std::make_unique<FlaskData>(id, name, std::move(*ports), *volume, *tx, txScale));
    return true;
}

template <>
bool LabwareRepository::add<LabwareType::ADAPTOR>(const LabwareId id, const def::Object& definition)
{
    const auto name    = definition.getDefaultProperty(def::Labware::Name, "?");
    auto       ports   = definition.getProperty(def::Labware::Ports, def::parse<std::vector<LabwarePort>>);
    const auto volume  = definition.getProperty(def::Labware::Volume, def::parse<Amount<Unit::LITER>>);
    const auto tx      = definition.getProperty(def::Labware::Texture);
    const auto txScale = definition.getDefaultProperty(def::Labware::TextureScale, 1.0f, def::parseUnsigned<float_s>);

    if (not(volume && ports && tx)) {
        Log(this).error("Incomplete adaptor definition at: {0}.", definition.getLocationName());
        return false;
    }

    if (not checkTextureFile(*tx, definition.getLocation()))
        return false;

    labware.emplace(id, std::make_unique<AdaptorData>(id, name, std::move(*ports), *volume, *tx, txScale));
    return true;
}

template <>
bool LabwareRepository::add<LabwareType::CONDENSER>(const LabwareId id, const def::Object& definition)
{
    const auto name    = definition.getDefaultProperty(def::Labware::Name, "?");
    auto       ports   = definition.getProperty(def::Labware::Ports, def::parse<std::vector<LabwarePort>>);
    const auto volume  = definition.getProperty(def::Labware::Volume, def::parse<Amount<Unit::LITER>>);
    const auto length  = definition.getProperty(def::Labware::Length, def::parse<Amount<Unit::METER>>);
    const auto effic   = definition.getProperty(def::Labware::Efficiency, def::parse<Amount<Unit::PER_METER>>);
    const auto tx      = definition.getProperty(def::Labware::Texture);
    const auto inner   = definition.getProperty(def::Labware::InnerMask);
    const auto coolant = definition.getProperty(def::Labware::CoolantMask);
    const auto txScale = definition.getDefaultProperty(def::Labware::TextureScale, 1.0f, def::parseUnsigned<float_s>);

    if (not(volume && length && effic && ports && tx && inner && coolant)) {
        Log(this).error("Incomplete condenser definition at: {0}.", definition.getLocationName());
        return false;
    }

    if (not(checkTextureFile(*tx, definition.getLocation()) &&
            checkTextureFile(*inner, definition.getLocation()) &&
            checkTextureFile(*coolant, definition.getLocation())))
        return false;

    labware.emplace(
        id,
        std::make_unique<CondenserData>(
            id, name, std::move(*ports), *volume, *length, *effic, *tx, *inner, *coolant, txScale));
    return true;
}

template <>
bool LabwareRepository::add<LabwareType::HEATSOURCE>(const LabwareId id, const def::Object& definition)
{
    const auto name    = definition.getDefaultProperty(def::Labware::Name, "?");
    auto       ports   = definition.getProperty(def::Labware::Ports, def::parse<std::vector<LabwarePort>>);
    const auto power   = definition.getProperty(def::Labware::Power, def::parse<Amount<Unit::WATT>>);
    const auto tx      = definition.getProperty(def::Labware::Texture);
    const auto txScale = definition.getDefaultProperty(def::Labware::TextureScale, 1.0f, def::parseUnsigned<float_s>);

    if (not(power && ports && tx)) {
        Log(this).error("Incomplete heatsource definition at: {0}.", definition.getLocationName());
        return false;
    }

    if (not checkTextureFile(*tx, definition.getLocation()))
        return false;

    labware.emplace(id, std::make_unique<HeatsourceData>(id, name, std::move(*ports), *power, *tx, txScale));
    return true;
}

bool LabwareRepository::add(const def::Object& definition)
{
    static std::unordered_map<std::string_view, bool (LabwareRepository::*)(LabwareId, const def::Object&)> adders = {
        {     def::Labware::Flask,      &LabwareRepository::add<LabwareType::FLASK>},
        {   def::Labware::Adaptor,    &LabwareRepository::add<LabwareType::ADAPTOR>},
        { def::Labware::Condenser,  &LabwareRepository::add<LabwareType::CONDENSER>},
        {def::Labware::Heatsource, &LabwareRepository::add<LabwareType::HEATSOURCE>}
    };

    const auto it = adders.find(definition.getSpecifier());
    if (it == adders.end()) {
        Log(this).error(
            "Unknown labware specifier: '{0}', at: {1}.", definition.getSpecifier(), definition.getLocationName());
        return false;
    }

    const auto id = definition.getProperty(def::Labware::Id, def::parse<LabwareId>);
    if (not id)
        return false;

    if (labware.contains(*id)) {
        Log(this).error("Labware with duplicate id: '{0}', at: {1}.", *id, definition.getLocationName());
        return false;
    }

    const auto success = (this->*(it->second))(*id, definition);
    if (not success)
        return false;

    definition.logUnusedWarnings();
    return true;
}

bool LabwareRepository::contains(const LabwareId id) const { return labware.contains(id); }

const BaseLabwareData& LabwareRepository::at(const LabwareId id) const { return *labware.at(id); }

size_t LabwareRepository::totalDefinitionCount() const { return labware.size(); }

LabwareRepository::Iterator LabwareRepository::begin() const { return labware.begin(); }

LabwareRepository::Iterator LabwareRepository::end() const { return labware.end(); }

size_t LabwareRepository::size() const { return labware.size(); }

void LabwareRepository::clear() { labware.clear(); }
