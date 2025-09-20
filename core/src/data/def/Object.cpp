#include "data/def/Object.hpp"

#include "data/DataStore.hpp"

using namespace def;

Object::Object(
    const DefinitionType              type,
    std::string&&                     identifier,
    std::string&&                     specifier,
    utils::StringMap<std::string>&&   properties,
    utils::StringMap<Object>&&        ilSubDefs,
    utils::StringMap<const Object*>&& outlineSubDefs,
    def::Location&&                   location) noexcept :
    type(type),
    identifier(std::move(identifier)),
    specifier(std::move(specifier)),
    location(std::move(location)),
    properties(std::move(properties)),
    ilSubDefs(std::move(ilSubDefs)),
    outlineSubDefs(std::move(outlineSubDefs))
{}

DefinitionType Object::getType() const { return type; }

const std::string& Object::getIdentifier() const { return identifier; }

const std::string& Object::getSpecifier() const { return specifier; }

const def::Location& Object::getLocation() const { return location; }

std::string Object::getLocationName() const { return location.toString(); }

void Object::logUnusedWarnings() const
{
    for (const auto& [k, _] : properties)
        if (not accessedProperties.contains(k))
            Log(this).warn("Unused property: '{}', at: {}.", k, location.toString());

    for (const auto& [k, _] : ilSubDefs)
        if (not accessedSubDefs.contains(k))
            Log(this).warn("Unused sub-definition for property: '{}', at: {}.", k, location.toString());

    for (const auto& [k, _] : outlineSubDefs)
        if (not accessedSubDefs.contains(k))
            Log(this).warn("Unused sub-definition for property: '{}', at: {}.", k, location.toString());
}

std::optional<std::string> Object::getOptionalProperty(const std::string_view key) const
{
    auto it = properties.find(key);
    if (it == properties.end())
        return std::nullopt;

    accessedProperties.emplace(key);
    return it->second;
}

std::optional<std::string> Object::getProperty(const std::string_view key) const
{
    const auto prop = getOptionalProperty(key);
    if (not prop)
        Log(this).error("Missing required property: '{}', at: {}.", key, location.toString());

    return prop;
}

std::string Object::getDefaultProperty(const std::string_view key, std::string&& defaultValue) const
{
    const auto prop = getOptionalProperty(key);
    return prop ? *prop : defaultValue;
}

const Object* Object::getOptionalDefinition(const std::string_view key) const
{
    const auto ilDef = ilSubDefs.find(key);
    if (ilDef != ilSubDefs.end()) {
        accessedSubDefs.emplace(key);
        return &ilDef->second;
    }

    const auto outlineDef = outlineSubDefs.find(key);
    if (outlineDef != outlineSubDefs.end()) {
        accessedSubDefs.emplace(key);
        return outlineDef->second;
    }

    return nullptr;
}

const Object* Object::getDefinition(const std::string_view key) const
{
    const auto def = getOptionalDefinition(key);
    if (def == nullptr)
        Log(this).error("Missing required sub-definition: '{}', at: {}.", key, location.toString());

    return def;
}
