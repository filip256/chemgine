#pragma once

#include <string>
#include <unordered_map>

#include "BaseLabwareData.hpp"

class LabwareDataTable
{
private:
	std::unordered_map<LabwareIdType, BaseLabwareData*> table;

public:
	LabwareDataTable() = default;
	LabwareDataTable(const LabwareDataTable&) = delete;
	~LabwareDataTable() noexcept;

	bool loadFromFile(const std::string& path);

	const BaseLabwareData& at(const LabwareIdType id) const;
};