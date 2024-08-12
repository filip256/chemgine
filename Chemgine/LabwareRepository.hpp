#pragma once

#include <string>
#include <unordered_map>

#include "BaseLabwareData.hpp"
#include "DefinitionObject.hpp"

class LabwareRepository
{
private:
	std::unordered_map<LabwareId, const BaseLabwareData*> table;

	template <LabwareType T>
	bool add(DefinitionObject&& definition) = delete;

public:
	LabwareRepository() = default;
	LabwareRepository(const LabwareRepository&) = delete;
	~LabwareRepository() noexcept;

	bool add(DefinitionObject&& definition);

	const BaseLabwareData& at(const LabwareId id) const;
};
