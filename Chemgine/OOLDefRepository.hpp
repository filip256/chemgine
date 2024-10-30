#pragma once

#include <string>
#include <memory>
#include <unordered_map>

namespace Def
{
	class Object;
}

class OOLDefRepository
{
private:
	std::unordered_map<std::string, std::unique_ptr<const Def::Object>> definitions;

public:
	OOLDefRepository() = default;
	OOLDefRepository(const OOLDefRepository&) = delete;
	OOLDefRepository(OOLDefRepository&&) = default;

	const Def::Object* add(Def::Object&& definition);

	const Def::Object* getDefinition(const std::string& identifier) const;

	void clear();
};
