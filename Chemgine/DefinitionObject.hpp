#pragma once

#include "DefinitionLocation.hpp"
#include "Log.hpp"

#include <unordered_map>
#include <optional>

enum class DefinitionType
{
	NONE,
	SPLINE,
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

public:
	DefinitionObject(
		const DefinitionType type,
		std::string&& identifier,
		std::string&& specifier,
		std::unordered_map<std::string, std::string>&& properties,
		DefinitionLocation&& location
	) noexcept;

	DefinitionObject(const DefinitionObject&) = delete;
	DefinitionObject(DefinitionObject&&) = default;

	DefinitionObject& operator=(DefinitionObject&&) = default;

	const std::string& getIdentifier() const;
	const std::string& getSpecifier() const;

	/// <summary>
	/// Returns the name of the location where the object was defined.
	/// </summary>
	std::string getLocationName() const;

	/// <summary>
	/// Returns a map containing only the properties which haven't been pulled yet.
	/// </summary>
	const std::unordered_map<std::string, std::string>& getRemainingProperties() const;

	/// <summary>
	/// Extracts and returns the property with the given key.
	/// If the property isn't found, an error message is logged.
	/// </summary>
	std::optional<std::string> pullProperty(const std::string& key);

	/// <summary>
	/// Extracts and returns the property with the given key (if found).
	/// </summary>
	std::optional<std::string> pullOptionalProperty(const std::string& key);

	/// <summary>
	/// Extracts and returns the property with the given key (if found).
	/// If the property isn't found the given default string is returned.
	/// </summary>
	std::string pullDefaultProperty(const std::string& key, std::string&& defaultValue);

	/// <summary>
	/// Extracts the property with the given key and returns its parsed value.
	/// If the property isn't found or parsing fails, an error message is logged.
	/// </summary>
	template<typename T, typename... Args>
	std::optional<T> pullProperty(
		const std::string& key,
		std::optional<T> (*parser)(const std::string&, Args...),
		Args&&... parserArgs);

	/// <summary>
	/// Extracts the property with the given key (if found) and returns its parsed value.
	/// If the parsing fails, a warning message is logged.
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
};

template<typename T, typename... Args>
std::optional<T> DefinitionObject::pullProperty(
	const std::string& key,
	std::optional<T>(*parser)(const std::string&, Args...),
	Args&&... parserArgs)
{
	const auto strProp = pullProperty(key);
	if (strProp.has_value() == false)
		return std::nullopt;

	const auto parsed = parser(*strProp, std::forward<Args>(parserArgs)...);
	if (parsed.has_value() == false)
		Log(this).error("Failed to parse property: '{0} : {1}', at: {2}.", key, *strProp, location.toString());

	return parsed;
}

template<typename T, typename... Args>
std::optional<T> DefinitionObject::pullOptionalProperty(
	const std::string& key,
	std::optional<T>(*parser)(const std::string&, Args...),
	Args&&... parserArgs)
{
	const auto strProp = pullOptionalProperty(key);
	if (strProp.has_value() == false)
		return std::nullopt;

	const auto parsed = parser(*strProp, std::forward<Args>(parserArgs)...);
	if (parsed.has_value() == false)
		Log(this).warn("Failed to parse optional property: '{0} : {1}', at: {2}.", key, *strProp, location.toString());

	return parsed;
}

template<typename T, typename... Args>
T DefinitionObject::pullDefaultProperty(
	const std::string& key,
	T&& defaultValue,
	std::optional<T>(*parser)(const std::string&, Args...),
	Args&&... parserArgs)
{
	const auto prop = pullOptionalProperty<T>(key, parser, std::forward<Args>(parserArgs)...);
	return prop.has_value() ? *prop : defaultValue;
}
