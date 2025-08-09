#pragma once

#include <string>
#include <memory>
#include <unordered_map>

namespace def
{
	class Object;
}

class OOLDefRepository
{
private:
	std::unordered_map<std::string, std::unique_ptr<const def::Object>> definitions;

public:
	OOLDefRepository() = default;
	OOLDefRepository(const OOLDefRepository&) = delete;
	OOLDefRepository(OOLDefRepository&&) = default;

	const def::Object* add(def::Object&& definition);

	const def::Object* getDefinition(const std::string& identifier) const;

	size_t totalDefinitionCount() const;

	void clear();
};
