#pragma once

#include "data/OOLDefRepository.hpp"
#include "data/def/Keywords.hpp"
#include "data/def/Object.hpp"
#include "data/def/Parsers.hpp"
#include "utils/Path.hpp"

template <>
class def::Parser<def::Object>
{
public:
    static std::optional<def::Object> parse(
        const std::string&                                  str,
        def::Location&&                                     location,
        const std::unordered_map<std::string, std::string>& includeAliases,
        const OOLDefRepository&                             oolDefinitions)
    {
        static const std::unordered_map<std::string_view, DefinitionType> typeMap{
            {    def::Types::Auto,     DefinitionType::AUTO},
            {    def::Types::Data,     DefinitionType::DATA},
            {    def::Types::Atom,     DefinitionType::ATOM},
            { def::Types::Radical,  DefinitionType::RADICAL},
            {def::Types::Molecule, DefinitionType::MOLECULE},
            {def::Types::Reaction, DefinitionType::REACTION},
            { def::Types::Labware,  DefinitionType::LABWARE},
        };

        const Log<Parser<def::Object>> log;

        if (str.starts_with('_') == false) {
            log.error("Missing definition begin symbol: '_', at: {0}.", location.toString());
            return std::nullopt;
        }

        auto idEnd   = std::string::npos;
        auto typeEnd = str.find('<', 1);
        if (typeEnd != std::string::npos) {
            idEnd = str.find('>', typeEnd + 1);
            if (idEnd == std::string::npos) {
                Log<Parser<def::Object>>().error("Missing identifier terminator: '>', at: {0}.", location.toString());
                return std::nullopt;
            }

            if (idEnd - typeEnd < 2) {
                log.error("Definition with empty identifier, at: {0}.", location.toString());
                return std::nullopt;
            }
        }
        else
            idEnd = typeEnd = str.find(':', 1);

        if (typeEnd == std::string::npos) {
            log.error("Malformed definition: '{0}', at: {1}.", str, location.toString());
            return std::nullopt;
        }

        const auto specifierBegin = str.find(':', idEnd);
        const auto specifierEnd   = str.find('{', specifierBegin + 1);
        if (specifierEnd == std::string::npos) {
            log.error("Malformed definition: '{0}', at: {1}.", str, location.toString());
            return std::nullopt;
        }

        const auto propertiesEnd = str.rfind('}');
        if (propertiesEnd == std::string::npos || propertiesEnd < specifierEnd) {
            log.error("Malformed definition: '{0}', at: {1}.", str, location.toString());
            return std::nullopt;
        }

        // parse type
        const auto typeStr = utils::strip(str.substr(1, typeEnd - 1));
        const auto typeIt  = typeMap.find(typeStr);
        if (typeIt == typeMap.end()) {
            log.error("Definition with unknown type: '{0}', at: {1}.", typeStr, location.toString());
            return std::nullopt;
        }
        const auto type = typeIt->second;

        // parse identifier (optional)
        const auto idStr = idEnd != typeEnd ? utils::strip(str.substr(typeEnd + 1, idEnd - typeEnd - 1)) : "";
        if (const auto illegalIdx = idStr.find_first_of(" .~;:'\"<>(){}~`!@#$%^&*()-+[]{}|?,/\\");
            illegalIdx != std::string::npos) {
            log.error(
                "Identifier: '{0}' contains illegal symbol: '{1}', at: {2}.",
                idStr,
                idStr[illegalIdx],
                location.toString());
            return std::nullopt;
        }

        // parse specifier
        auto specifierStr = utils::strip(str.substr(specifierBegin + 1, specifierEnd - specifierBegin - 1));
        if (specifierStr.empty()) {
            log.error("Definition with missing specifier: '{0}', at: {1}.", str, location.toString());
            return std::nullopt;
        }

        // parse properties
        const auto propertiesStr = str.substr(specifierEnd + 1, propertiesEnd - specifierEnd - 1);
        if (propertiesStr.empty()) {
            log.error("Definition with missing properties block: '{0}', at: {1}.", str, location.toString());
            return std::nullopt;
        }

        const auto                           props = utils::split(propertiesStr, ',', "{[(", "}])", true);
        utils::StringMap<std::string>        properties;
        utils::StringMap<def::Object>        ilSubDefs;
        utils::StringMap<const def::Object*> oolSubDefs;

        for (size_t i = 0; i < props.size(); ++i) {
            const size_t nameEnd = props[i].find(':');
            if (nameEnd == std::string::npos) {
                log.error("Definition with malformed property: '{0}', at: {1}.", props[i], location.toString());
                return std::nullopt;
            }

            auto name = utils::strip(props[i].substr(0, nameEnd));
            if (name.empty()) {
                log.error("Definition property with missing name: '{0}', at: {1}.", props[i], location.toString());
                return std::nullopt;
            }
            if (const auto illegalIdx = name.find_first_of(" .~;:'\"<>(){}~`!@#$%^&*()-+[]{}|?,/\\");
                illegalIdx != std::string::npos) {
                log.error(
                    "Property name: '{0}' contains illegal symbol: '{1}', at: {2}.",
                    name,
                    name[illegalIdx],
                    location.toString());
                return std::nullopt;
            }

            auto value = utils::strip(props[i].substr(nameEnd + 1));
            if (value.empty()) {
                log.error("Definition property with missing value: '{0}', at: {1}.", props[i], location.toString());
                return std::nullopt;
            }

            // in-line sub-definition
            if (value.starts_with('_')) {
                auto subDef = def::parse<def::Object>(value, utils::copy(location), includeAliases, oolDefinitions);
                if (not subDef) {
                    log.error("Failed to parse in-line sub-definition: '{0}', at: {1}.", value, location.toString());
                    return std::nullopt;
                }

                ilSubDefs.emplace(std::move(name), std::move(*subDef));
                continue;
            }

            // out-of-line sub-definition
            if (value.starts_with('$')) {
                // expand include aliases
                if (const auto aliasEnd = value.find('@', 1); aliasEnd != std::string::npos) {
                    const auto alias = value.substr(1, aliasEnd - 1);
                    if (alias.empty()) {
                        log.error("Empty include alias on value: '{0}', at: {1}.", value, location.toString());
                        return std::nullopt;
                    }

                    const auto aliasIt = includeAliases.find(alias);
                    if (aliasIt == includeAliases.end()) {
                        log.error("Undefined include alias: '{0}', at: {1}.", alias, location.toString());
                        return std::nullopt;
                    }

                    value.replace(1, alias.size(), aliasIt->second);
                }
                else
                    value.insert(1, location.getFile() + '@');  // append local address

                const auto* definition = oolDefinitions.getDefinition(value.substr(1));
                if (definition == nullptr) {
                    log.error("Unknown out-of-line definition identifier: '{0}', at: {1}.", value, location.toString());
                    return std::nullopt;
                }

                oolSubDefs.emplace(std::move(name), definition);
                continue;
            }

            // relative path
            if (value.starts_with("~/"))
                value = utils::combinePaths(utils::extractDirName(location.getFile()), value.substr(1));

            // normal properties
            properties.emplace(std::move(name), std::move(value));
        }

        return std::optional<def::Object>(
            std::in_place,
            type,
            idStr.size() ? location.getFile() + '@' + idStr : "",
            std::move(specifierStr),
            std::move(properties),
            std::move(ilSubDefs),
            std::move(oolSubDefs),
            std::move(location));
    }
};
