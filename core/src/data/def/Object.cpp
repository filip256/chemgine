#include "data/def/Object.hpp"
#include "data/DataStore.hpp"

using namespace def;

Object::Object(
	const DefinitionType type,
	std::string&& identifier,
	std::string&& specifier,
	std::unordered_map<std::string, std::string>&& properties,
	std::unordered_map<std::string, Object>&& ilSubDefs,
	std::unordered_map<std::string, const Object*>&& oolSubDefs,
	def::Location&& location
) noexcept :
	type(type),
	identifier(std::move(identifier)),
	specifier(std::move(specifier)),
	properties(std::move(properties)),
	ilSubDefs(std::move(ilSubDefs)),
	oolSubDefs(std::move(oolSubDefs)),
	location(std::move(location))
{}

DefinitionType Object::getType() const
{
	return type;
}

const std::string& Object::getIdentifier() const
{
	return identifier;
}

const std::string& Object::getSpecifier() const
{
	return specifier;
}

const def::Location& Object::getLocation() const
{
	return location;
}

std::string Object::getLocationName() const
{
	return location.toString();
}

void Object::logUnusedWarnings() const
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

std::optional<std::string> Object::getOptionalProperty(const std::string& key) const
{
	auto it = properties.find(key);
	if (it == properties.end())
		return std::nullopt;

	accessedProperties.emplace(key);
	return it->second;
}

std::optional<std::string> Object::getProperty(const std::string& key) const
{
	const auto prop = getOptionalProperty(key);
	if (not prop)
		Log(this).error("Missing required property: '{0}', at: {1}.", key, location.toString());

	return prop;
}

std::string Object::getDefaultProperty(const std::string& key, std::string&& defaultValue) const
{
	const auto prop = getOptionalProperty(key);
	return prop ? *prop : defaultValue;
}

const Object* Object::getOptionalDefinition(const std::string& key) const
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

const Object* Object::getDefinition(const std::string& key) const
{
	const auto def = getOptionalDefinition(key);
	if (def == nullptr)
		Log(this).error("Missing required sub-definition: '{0}', at: {1}.", key, location.toString());

	return def;
}
