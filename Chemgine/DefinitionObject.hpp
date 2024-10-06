#pragma once

#include "DefinitionLocation.hpp"
#include "CountedRef.hpp"
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
	LABWARE,
};

class DefinitionObject
{
private:
	const DefinitionType type;
	const std::string identifier;
	const std::string specifier;
	const DefinitionLocation location;
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

	DefinitionType getType() const;
	const std::string& getIdentifier() const;
	const std::string& getSpecifier() const;

	const DefinitionLocation& getLocation() const;
	std::string getLocationName() const;

	/// <summary>
	/// Returns a map containing only the properties which haven't been pulled yet.
	/// </summary>
	const std::unordered_map<std::string, std::string>& getRemainingProperties() const;
	const std::unordered_map<std::string, DefinitionObject>& getILSubDefs() const;
	const std::unordered_map<std::string, const DefinitionObject*>& getOOLSubDefs() const;

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
	template<typename T, typename D, typename... Args>
	T getDefaultProperty(
		const std::string& key,
		D&& defaultValue,
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
	std::optional<CountedRef<const T>> getDefinition(
		const std::string& key,
		std::optional<CountedRef<const T>>(*parser)(const DefinitionObject&, Args...),
		Args&&... parserArgs) const;

	/// <summary>
	/// Extracts the definition with the given key (if found) and returns its parsed value.
	/// If the parsing fails, a warning message is logged.
	/// </summary>
	template<typename T, typename... Args>
	std::optional<CountedRef<const T>> getOptionalDefinition(
		const std::string& key,
		std::optional<CountedRef<const T>>(*parser)(const DefinitionObject&, Args...),
		Args&&... parserArgs) const;
};

template<typename T, typename... Args>
std::optional<T> DefinitionObject::getProperty(
	const std::string& key,
	std::optional<T>(*parser)(const std::string&, Args...),
	Args&&... parserArgs) const
{
	const auto strProp = getProperty(key);
	if (not strProp)
		return std::nullopt;

	const auto parsed = parser(*strProp, std::forward<Args>(parserArgs)...);
	if (not parsed)
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
	if (not strProp)
		return std::nullopt;

	const auto parsed = parser(*strProp, std::forward<Args>(parserArgs)...);
	if (not parsed)
		Log(this).warn("Failed to parse optional property: '{0} : {1}', at: {2}.", key, *strProp, location.toString());

	return parsed;
}

template<typename T, typename D, typename... Args>
T DefinitionObject::getDefaultProperty(
	const std::string& key,
	D&& defaultValue,
	std::optional<T>(*parser)(const std::string&, Args...),
	Args&&... parserArgs) const
{
	const auto prop = getOptionalProperty<T>(key, parser, std::forward<Args>(parserArgs)...);
	return prop ? *prop : defaultValue;
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
std::optional<CountedRef<const T>> DefinitionObject::getOptionalDefinition(
	const std::string& key,
	std::optional<CountedRef<const T>>(*parser)(const DefinitionObject&, Args...),
	Args&&... parserArgs) const
{
	const auto* def = getOptionalDefinition(key);
	if (def == nullptr)
		return std::nullopt;

	auto parsed = parser(*def, std::forward<Args>(parserArgs)...);
	if (not parsed)
		Log(this).warn("Failed to parse optional sub-definition for: '{0}', at: {1}.", key, location.toString());

	return std::move(parsed);
}

template<typename T, typename... Args>
std::optional<CountedRef<const T>> DefinitionObject::getDefinition(
	const std::string& key,
	std::optional<CountedRef<const T>>(*parser)(const DefinitionObject&, Args...),
	Args&&... parserArgs) const
{
	const auto* def = getDefinition(key);
	if (def == nullptr)
		return std::nullopt;

	auto parsed = parser(*def, std::forward<Args>(parserArgs)...);
	if (not parsed)
		Log(this).error("Failed to parse sub-definition for: '{0}', at: {1}.", key, location.toString());

	return std::move(parsed);
}
