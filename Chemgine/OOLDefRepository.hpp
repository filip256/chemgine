#pragma once

#include <string>
#include <memory>
#include <unordered_map>

class DefinitionObject;

class OOLDefRepository
{
private:
	std::unordered_map<std::string, std::unique_ptr<const DefinitionObject>> definitions;

public:
	OOLDefRepository() = default;
	OOLDefRepository(const OOLDefRepository&) = delete;
	OOLDefRepository(OOLDefRepository&&) = default;

	const DefinitionObject* add(DefinitionObject&& definition);

	const DefinitionObject* getDefinition(const std::string& identifier) const;

	void clear();
};
