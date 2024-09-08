#pragma once

#include "DefinitionLocation.hpp"
#include "OOLDefRepository.hpp"
#include "Parsers.hpp"
#include "Keywords.hpp"
#include "Log.hpp"

#include <unordered_map>
#include <optional>

enum class DefinitionType
{
	AUTO,
	DATA,
	ATOM,
	RADICAL,
	MOLECULE,
	REACTION,
	LABWARE
};

class DefinitionObject
{
public:
	DefinitionType type;

private:
	std::string identifier;
	std::string specifier;
	DefinitionLocation location;
	std::unordered_map<std::string, std::string> properties;
	std::unordered_map<std::string, DefinitionObject> ilSubDefs;
	std::unordered_map<std::string, const DefinitionObject*> oolSubDefs;

public:
	DefinitionObject(
		const DefinitionType type,
		std::string&& identifier,
		std::string&& specifier,
		std::unordered_map<std::string, std::string>&& properties,
		std::unordered_map<std::string, DefinitionObject>&& ilSubDefs,
		std::unordered_map<std::string, const DefinitionObject*>&& oolSubDefs,
		DefinitionLocation&& location
	) noexcept;

	DefinitionObject(const DefinitionObject&) = delete;
	DefinitionObject(DefinitionObject&&) = default;

	DefinitionObject& operator=(DefinitionObject&&) = default;

	const std::string& getIdentifier() const;
	const std::string& getSpecifier() const;

	const DefinitionLocation& getLocation() const;
	std::string getLocationName() const;

	/// <summary>
	/// Returns a map containing only the properties which haven't been pulled yet.
	/// </summary>
	const std::unordered_map<std::string, std::string>& getRemainingProperties() const;

	/// <summary>
	/// Extracts and returns the property with the given key.
	/// If the property isn't found, an error message is logged.
	/// </summary>
	std::optional<std::string> getProperty(const std::string& key) const;

	/// <summary>
	/// Extracts and returns the property with the given key (if found).
	/// </summary>
	std::optional<std::string> getOptionalProperty(const std::string& key) const;

	/// <summary>
	/// Extracts and returns the property with the given key (if found).
	/// If the property isn't found the given default string is returned.
	/// </summary>
	std::string getDefaultProperty(const std::string& key, std::string&& defaultValue) const;

	/// <summary>
	/// Extracts the property with the given key and returns its parsed value.
	/// If the property isn't found or parsing fails, an error message is logged.
	/// </summary>
	template<typename T, typename... Args>
	std::optional<T> getProperty(
		const std::string& key,
		std::optional<T> (*parser)(const std::string&, Args...),
		Args&&... parserArgs) const;

	/// <summary>
	/// Extracts the property with the given key (if found) and returns its parsed value.
	/// If the parsing fails, a warning message is logged.
	/// </summary>
	template<typename T, typename... Args>
	std::optional<T> getOptionalProperty(
		const std::string& key,
		std::optional<T>(*parser)(const std::string&, Args...),
		Args&&... parserArgs) const;

	/// <summary>
	/// Extracts the property with the given key (if found) and returns its parsed value.
	/// If the parsing fails, a warning message is logged.
	/// If the property isn't found or parsing fails, the given default value is returned.
	/// </summary>
	template<typename T, typename... Args>
	T getDefaultProperty(
		const std::string& key,
		T&& defaultValue,
		std::optional<T>(*parser)(const std::string&, Args...),
		Args&&... parserArgs) const;

	/// <summary>
	/// Similar to the corresponding "get" variant but also removes the property.
	/// </summary>
	std::optional<std::string> pullProperty(const std::string& key);

	/// <summary>
	/// Similar to the corresponding "get" variant but also removes the property.
	/// </summary>
	std::optional<std::string> pullOptionalProperty(const std::string& key);

	/// <summary>
	/// Similar to the corresponding "get" variant but also removes the property.
	/// </summary>
	std::string pullDefaultProperty(const std::string& key, std::string&& defaultValue);

	/// <summary>
	/// Similar to the corresponding "get" variant but also removes the property.
	/// </summary>
	template<typename T, typename... Args>
	std::optional<T> pullProperty(
		const std::string& key,
		std::optional<T>(*parser)(const std::string&, Args...),
		Args&&... parserArgs);

	/// <summary>
	/// Similar to the corresponding "get" variant but also removes the property.
	/// </summary>
	template<typename T, typename... Args>
	std::optional<T> pullOptionalProperty(
		const std::string& key,
		std::optional<T>(*parser)(const std::string&, Args...),
		Args&&... parserArgs);

	/// <summary>
	/// Extracts the property with the given key (if found) and returns its parsed value.
	/// If the parsing fails, a warning message is logged.
	/// If the property isn't found or parsing fails, the given default value is returned.
	/// </summary>
	template<typename T, typename... Args>
	T pullDefaultProperty(
		const std::string& key,
		T&& defaultValue,
		std::optional<T>(*parser)(const std::string&, Args...),
		Args&&... parserArgs);

	/// <summary>
	/// Extracts and returns the definition with the given key.
	/// If the definition isn't found, an error message is logged.
	/// </summary>
	const DefinitionObject* getDefinition(const std::string& key) const;

	/// <summary>
	/// Extracts and returns the definition with the given key (if found).
	/// </summary>
	const DefinitionObject* getOptionalDefinition(const std::string& key) const;

	/// <summary>
	/// Extracts the definition with the given key and returns its parsed value.
	/// If the definition isn't found or parsing fails, an error message is logged.
	/// </summary>
	template<typename T, typename... Args>
	std::optional<T> getDefinition(
		const std::string& key,
		std::optional<T>(*parser)(const DefinitionObject&, Args...),
		Args&&... parserArgs) const;

	/// <summary>
	/// Extracts the definition with the given key (if found) and returns its parsed value.
	/// If the parsing fails, a warning message is logged.
	/// </summary>
	template<typename T, typename... Args>
	std::optional<T> getOptionalDefinition(
		const std::string& key,
		std::optional<T>(*parser)(const DefinitionObject&, Args...),
		Args&&... parserArgs) const;
};

template<typename T, typename... Args>
std::optional<T> DefinitionObject::getProperty(
	const std::string& key,
	std::optional<T>(*parser)(const std::string&, Args...),
	Args&&... parserArgs) const
{
	const auto strProp = getProperty(key);
	if (strProp.has_value() == false)
		return std::nullopt;

	const auto parsed = parser(*strProp, std::forward<Args>(parserArgs)...);
	if (parsed.has_value() == false)
		Log(this).error("Failed to parse property: '{0} : {1}', at: {2}.", key, *strProp, location.toString());

	return parsed;
}

template<typename T, typename... Args>
std::optional<T> DefinitionObject::getOptionalProperty(
	const std::string& key,
	std::optional<T>(*parser)(const std::string&, Args...),
	Args&&... parserArgs) const
{
	const auto strProp = getOptionalProperty(key);
	if (strProp.has_value() == false)
		return std::nullopt;

	const auto parsed = parser(*strProp, std::forward<Args>(parserArgs)...);
	if (parsed.has_value() == false)
		Log(this).warn("Failed to parse optional property: '{0} : {1}', at: {2}.", key, *strProp, location.toString());

	return parsed;
}

template<typename T, typename... Args>
T DefinitionObject::getDefaultProperty(
	const std::string& key,
	T&& defaultValue,
	std::optional<T>(*parser)(const std::string&, Args...),
	Args&&... parserArgs) const
{
	const auto prop = getOptionalProperty<T>(key, parser, std::forward<Args>(parserArgs)...);
	return prop.has_value() ? *prop : defaultValue;
}

template<typename T, typename... Args>
std::optional<T> DefinitionObject::pullProperty(
	const std::string& key,
	std::optional<T>(*parser)(const std::string&, Args...),
	Args&&... parserArgs)
{
	const auto prop = getProperty(key, parser, std::forward<Args>(parserArgs)...);
	properties.erase(key);
	return prop;
}

template<typename T, typename... Args>
std::optional<T> DefinitionObject::pullOptionalProperty(
	const std::string& key,
	std::optional<T>(*parser)(const std::string&, Args...),
	Args&&... parserArgs)
{
	const auto prop = getOptionalProperty(key, parser, std::forward<Args>(parserArgs)...);
	properties.erase(key);
	return prop;
}

template<typename T, typename... Args>
T DefinitionObject::pullDefaultProperty(
	const std::string& key,
	T&& defaultValue,
	std::optional<T>(*parser)(const std::string&, Args...),
	Args&&... parserArgs)
{
	const auto prop = getDefaultProperty(key, std::move(defaultValue), parser, std::forward<Args>(parserArgs)...);
	properties.erase(key);
	return prop;
}

template<typename T, typename... Args>
std::optional<T> DefinitionObject::getOptionalDefinition(
	const std::string& key,
	std::optional<T>(*parser)(const DefinitionObject&, Args...),
	Args&&... parserArgs) const
{
	const auto* def = getOptionalDefinition(key);
	if (def == nullptr)
		return std::nullopt;

	const auto parsed = parser(*def, std::forward<Args>(parserArgs)...);
	if (parsed.has_value() == false)
		Log(this).warn("Failed to parse optional sub-definition for: '{0}', at: {1}.", key, location.toString());

	return parsed;
}

template<typename T, typename... Args>
std::optional<T> DefinitionObject::getDefinition(
	const std::string& key,
	std::optional<T>(*parser)(const DefinitionObject&, Args...),
	Args&&... parserArgs) const
{
	const auto* def = getDefinition(key);
	if (def == nullptr)
		return std::nullopt;

	const auto parsed = parser(*def, std::forward<Args>(parserArgs)...);
	if (parsed.has_value() == false)
		Log(this).error("Failed to parse sub-definition for: '{0}', at: {1}.", key, location.toString());

	return parsed;
}


template <>
class Def::Parser<DefinitionObject>
{
public:
	static std::optional<DefinitionObject> parse(
		const std::string& str,
		DefinitionLocation&& location,
		const std::unordered_map<std::string, std::string>& includeAliases,
		const OOLDefRepository& oolDefinitions)
	{
		static const std::unordered_map<std::string, DefinitionType> typeMap
		{
			{Keywords::Types::Auto, DefinitionType::AUTO},
			{Keywords::Types::Data, DefinitionType::DATA},
			{Keywords::Types::Atom, DefinitionType::ATOM},
			{Keywords::Types::Radical, DefinitionType::RADICAL},
			{Keywords::Types::Molecule, DefinitionType::MOLECULE},
			{Keywords::Types::Reaction, DefinitionType::REACTION},
			{Keywords::Types::Labware, DefinitionType::LABWARE},
		};

		const Log<Parser<DefinitionObject>> log;

		if (str.starts_with('_') == false)
		{
			log.error("Missing definition begin symbol: '_', at: {0}.", location.toString());
			return std::nullopt;
		}

		auto idEnd = std::string::npos;
		auto typeEnd = str.find('<', 1);
		if (typeEnd != std::string::npos)
		{
			idEnd = str.find('>', typeEnd + 1);
			if (idEnd == std::string::npos)
			{
				Log<Parser<DefinitionObject>>().error("Missing identifier terminator: '>', at: {0}.", location.toString());
				return std::nullopt;
			}

			if (idEnd - typeEnd < 2)
			{
				log.error("Definition with empty identifier, at: {0}.", location.toString());
				return std::nullopt;
			}
		}
		else
			idEnd = typeEnd = str.find(':', 1);

		if (typeEnd == std::string::npos)
		{
			log.error("Malformed definition: '{0}', at: {1}.", str, location.toString());
			return std::nullopt;
		}

		const auto specifierBegin = str.find(':', idEnd);
		const auto specifierEnd = str.find('{', specifierBegin + 1);
		if (specifierEnd == std::string::npos)
		{
			log.error("Malformed definition: '{0}', at: {1}.", str, location.toString());
			return std::nullopt;
		}

		const auto propertiesEnd = str.rfind('}');
		if (propertiesEnd == std::string::npos || propertiesEnd < specifierEnd)
		{
			log.error("Malformed definition: '{0}', at: {1}.", str, location.toString());
			return std::nullopt;
		}

		// parse type
		const auto typeStr = Utils::strip(str.substr(1, typeEnd - 1));
		const auto typeIt = typeMap.find(typeStr);
		if (typeIt == typeMap.end())
		{
			log.error("Definition with unknown type: '{0}', at: {1}.", typeStr, location.toString());
			return std::nullopt;
		}
		const auto type = typeIt->second;

		// parse identifier (optional)
		const auto idStr = idEnd != typeEnd ? Utils::strip(str.substr(typeEnd + 1, idEnd - typeEnd - 1)) : "";
		if (const auto illegalIdx = idStr.find_first_of(" .~;:'\"<>(){}~`!@#$%^&*()-+[]{}|?,/\\"); illegalIdx != std::string::npos)
		{
			log.error("Identifier: '{0}' contains illegal symbol: '{1}', at: {2}.", idStr, idStr[illegalIdx], location.toString());
			return std::nullopt;
		}

		// parse specifier
		auto specifierStr = Utils::strip(str.substr(specifierBegin + 1, specifierEnd - specifierBegin - 1));
		if (specifierStr.empty())
		{
			log.error("Definition with missing specifier: '{0}', at: {1}.", str, location.toString());
			return std::nullopt;
		}

		// parse properties
		const auto propertiesStr = str.substr(specifierEnd + 1, propertiesEnd - specifierEnd - 1);
		if (propertiesStr.empty())
		{
			log.error("Definition with missing properties block: '{0}', at: {1}.", str, location.toString());
			return std::nullopt;
		}

		const auto props = Utils::split(propertiesStr, ',', '{', '}', true);
		std::unordered_map<std::string, std::string> properties;
		std::unordered_map<std::string, DefinitionObject> ilSubDefs;
		std::unordered_map<std::string, const DefinitionObject*> oolSubDefs;

		for (size_t i = 0; i < props.size(); ++i)
		{
			const size_t nameEnd = props[i].find(':');
			if (nameEnd == std::string::npos)
			{
				log.error("Definition with malformed property: '{0}', at: {1}.", props[i], location.toString());
				return std::nullopt;
			}

			auto name = Utils::strip(props[i].substr(0, nameEnd));
			if (name.empty())
			{
				log.error("Definition property with missing name: '{0}', at: {1}.", props[i], location.toString());
				return std::nullopt;
			}

			auto value = Utils::strip(props[i].substr(nameEnd + 1));
			if (value.empty())
			{
				log.error("Definition property with missing value: '{0}', at: {1}.", props[i], location.toString());
				return std::nullopt;
			}

			// in-line sub-definition
			if (value.starts_with('_'))
			{
				auto subDef = Def::parse<DefinitionObject>(
					value, Utils::copy(location), includeAliases, oolDefinitions);
				if (subDef.has_value() == false)
				{
					log.error("Failed to parse in-line sub-definition: '{0}', at: {1}.", value, location.toString());
					return std::nullopt;
				}

				ilSubDefs.emplace(std::move(name), std::move(*subDef));
				continue;
			}

			// out-of-line sub-definition
			if (value.starts_with('$'))
			{
				// expand include aliases
				if (const auto aliasEnd = value.find('@', 1); aliasEnd != std::string::npos)
				{
					const auto alias = value.substr(1, aliasEnd - 1);
					if (alias.empty())
					{
						log.error("Empty include alias on value: '{0}', at: {1}.", value, location.toString());
						return std::nullopt;
					}

					const auto aliasIt = includeAliases.find(alias);
					if (aliasIt == includeAliases.end())
					{
						log.error("Undefined include alias: '{0}', at: {1}.", alias, location.toString());
						return std::nullopt;
					}

					value.replace(1, alias.size(), aliasIt->second);
				}

				const auto* definition = oolDefinitions.getDefinition(value.substr(1));
				if (definition == nullptr)
				{
					log.error("Unknown out-of-line definition identifier: '{0}', at: {1}.", value, location.toString());
					return std::nullopt;
				}

				oolSubDefs.emplace(std::move(name), definition);
				continue;
			}

			// normal properties
			properties.emplace(std::move(name), std::move(value));
		}

		return std::optional<DefinitionObject>(std::in_place,
			type,
			idStr.size() ? location.getFile() + '@' + idStr : "",
			std::move(specifierStr),
			std::move(properties),
			std::move(ilSubDefs),
			std::move(oolSubDefs),
			std::move(location)
		);
	}
};
