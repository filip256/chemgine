#include "DefinitionObject.hpp"
#include "DataStore.hpp"

DefinitionObject::DefinitionObject(
	const DefinitionType type,
	std::string&& identifier,
	std::string&& specifier,
	std::unordered_map<std::string, std::string>&& properties,
	DefinitionLocation&& location
) noexcept :
	type(type),
	identifier(std::move(identifier)),
	specifier(std::move(specifier)),
	properties(std::move(properties)),
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

std::string DefinitionObject::getLocationName() const
{
	return location.toString();
}

const std::unordered_map<std::string, std::string>& DefinitionObject::getRemainingProperties() const
{
	return properties;
}

std::optional<std::string> DefinitionObject::pullProperty(const std::string& key)
{
	const auto prop = pullOptionalProperty(key);
	if (prop.has_value() == false)
		Log(this).error("Missing required property: '{0}', at: {1}.", key, location.toString());

	return prop;
}

std::optional<std::string> DefinitionObject::pullOptionalProperty(const std::string& key)
{
	auto handle = properties.extract(key);
	if (handle.empty())
		return std::nullopt;

	return std::move(handle.mapped());
}

std::string DefinitionObject::pullDefaultProperty(const std::string& key, std::string&& defaultValue)
{
	const auto prop = pullOptionalProperty(key);
	return prop.has_value() ? *prop : defaultValue;
}
