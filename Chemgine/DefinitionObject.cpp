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

DefinitionObject::~DefinitionObject() noexcept
{
	logUnusedWarnings();
}

DefinitionType DefinitionObject::getType() const
{
	return type;
}

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

void DefinitionObject::logUnusedWarnings() const
{
	for (const auto& [k, _] : properties)
		if(not accessedProperties.contains(k))
			Log(this).warn("Unused property: '{0}', at: {1}.", k, location.toString());

	for (const auto& [k, _] : ilSubDefs)
		if (not accessedSubDefs.contains(k))
			Log(this).warn("Unused sub-definition for property: '{0}', at: {1}.", k, location.toString());

	for (const auto& [k, _] : oolSubDefs)
		if (not accessedSubDefs.contains(k))
			Log(this).warn("Unused sub-definition for property: '{0}', at: {1}.", k, location.toString());
}

std::optional<std::string> DefinitionObject::getOptionalProperty(const std::string& key) const
{
	auto it = properties.find(key);
	if (it == properties.end())
		return std::nullopt;

	accessedProperties.emplace(key);
	return it->second;
}

std::optional<std::string> DefinitionObject::getProperty(const std::string& key) const
{
	const auto prop = getOptionalProperty(key);
	if (not prop)
		Log(this).error("Missing required property: '{0}', at: {1}.", key, location.toString());

	return prop;
}

std::string DefinitionObject::getDefaultProperty(const std::string& key, std::string&& defaultValue) const
{
	const auto prop = getOptionalProperty(key);
	return prop ? *prop : defaultValue;
}

const DefinitionObject* DefinitionObject::getOptionalDefinition(const std::string& key) const
{
	const auto ilDef = ilSubDefs.find(key);
	if (ilDef != ilSubDefs.end())
	{
		accessedSubDefs.emplace(key);
		return &ilDef->second;
	}

	const auto oolDef = oolSubDefs.find(key);
	if (oolDef != oolSubDefs.end())
	{
		accessedSubDefs.emplace(key);
		return oolDef->second;
	}

	return nullptr;
}

const DefinitionObject* DefinitionObject::getDefinition(const std::string& key) const
{
	const auto def = getOptionalDefinition(key);
	if (def == nullptr)
		Log(this).error("Missing required sub-definition: '{0}', at: {1}.", key, location.toString());

	return def;
}
