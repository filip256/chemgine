#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include "BaseLabwareData.hpp"
#include "DefinitionObject.hpp"

class LabwareRepository
{
private:
	std::unordered_map<LabwareId, std::unique_ptr<const BaseLabwareData>> table;

	bool checkTextureFile(std::string path, const DefinitionLocation& location);

	template <LabwareType T>
	bool add(const LabwareId id, DefinitionObject&& definition) = delete;

public:
	LabwareRepository() = default;
	LabwareRepository(const LabwareRepository&) = delete;

	bool add(DefinitionObject&& definition);

	const BaseLabwareData& at(const LabwareId id) const;

	using Iterator = std::unordered_map<LabwareId, std::unique_ptr<const BaseLabwareData>>::const_iterator;
	Iterator begin() const;
	Iterator end() const;

	size_t size() const;
	void clear();
};
