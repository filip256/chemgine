#include "DefinitionObject.hpp"
#include "DataStore.hpp"

DefinitionObject::DefinitionObject(
	const DefinitionType type,
	std::string&& identifier,
	std::string&& specifier,
	std::unordered_map<std::string, std::string>&& properties,
	std::unordered_map<std::string, DefinitionObject>&& ilSubDefs,
	std::unordered_map<std::string, const DefinitionObject*>&& oolSubDefs,
	DefinitionLocation&& location
) noexcept :
	type(type),
	identifier(std::move(identifier)),
	specifier(std::move(specifier)),
	properties(std::move(properties)),
	ilSubDefs(std::move(ilSubDefs)),
	oolSubDefs(std::move(oolSubDefs)),
	location(std::move(location))
{}

const std::string& DefinitionObject::getIdentifier() const
{
	return identifier;
}

const std::string& DefinitionObject::getSpecifier() const
{
	return specifier;
}

const DefinitionLocation& DefinitionObject::getLocation() const
{
	return location;
}

std::string DefinitionObject::getLocationName() const
{
	return location.toString();
}

const std::unordered_map<std::string, std::string>& DefinitionObject::getRemainingProperties() const
{
	return properties;
}

std::optional<std::string> DefinitionObject::getProperty(const std::string& key) const
{
	const auto prop = getOptionalProperty(key);
	if (prop.has_value() == false)
		Log(this).error("Missing required property: '{0}', at: {1}.", key, location.toString());

	return prop;
}

std::optional<std::string> DefinitionObject::getOptionalProperty(const std::string& key) const
{
	auto handle = properties.find(key);
	if (handle == properties.end())
		return std::nullopt;

	return std::move(handle->second);
}

std::string DefinitionObject::getDefaultProperty(const std::string& key, std::string&& defaultValue) const
{
	const auto prop = getOptionalProperty(key);
	return prop.has_value() ? *prop : defaultValue;
}

std::optional<std::string> DefinitionObject::pullProperty(const std::string& key)
{
	const auto prop = getProperty(key);
	properties.erase(key);
	return prop;
}

std::optional<std::string> DefinitionObject::pullOptionalProperty(const std::string& key)
{
	const auto prop = getOptionalProperty(key);
	properties.erase(key);
	return prop;
}

std::string DefinitionObject::pullDefaultProperty(const std::string& key, std::string&& defaultValue)
{
	const auto prop = getDefaultProperty(key, std::move(defaultValue));
	properties.erase(key);
	return prop;
}

const DefinitionObject* DefinitionObject::getDefinition(const std::string& key) const
{
	const auto def = getOptionalDefinition(key);
	if (def == nullptr)
		Log(this).error("Missing required sub-definition: '{0}', at: {1}.", key, location.toString());

	return def;
}

const DefinitionObject* DefinitionObject::getOptionalDefinition(const std::string& key) const
{
	const auto ilDef = ilSubDefs.find(key);
	if (ilDef != ilSubDefs.end())
		return &ilDef->second;

	const auto oolDef = oolSubDefs.find(key);
	if (oolDef != oolSubDefs.end())
		return oolDef->second;

	return nullptr;
}
