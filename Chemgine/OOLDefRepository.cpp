#include "OOLDefRepository.hpp"
#include "DefinitionObject.hpp"

bool OOLDefRepository::add(DefinitionObject&& definition)
{
	auto temp = definition.getIdentifier();
	const auto success = definitions.emplace(std::move(temp),
		std::make_unique<const DefinitionObject>(std::move(definition)));
	if (success.second == false)
	{
		Log(this).error("Clash between existing OOL identifier: '{0}' and new definition at: {1}.", success.first->first, definition.getLocationName());
		return false;
	}

	return true;
}

const DefinitionObject* OOLDefRepository::getDefinition(const std::string& identifier) const
{
	const auto it = definitions.find(identifier);
	return it != definitions.end() ? it->second.get() : nullptr;
}
