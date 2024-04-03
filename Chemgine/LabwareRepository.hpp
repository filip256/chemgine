#pragma once

#include <string>
#include <unordered_map>

#include "BaseLabwareData.hpp"

class LabwareRepository
{
private:
	std::unordered_map<LabwareId, const BaseLabwareData*> table;

public:
	LabwareRepository() = default;
	LabwareRepository(const LabwareRepository&) = delete;
	~LabwareRepository() noexcept;

	bool loadFromFile(const std::string& path);

	const BaseLabwareData& at(const LabwareId id) const;
};
