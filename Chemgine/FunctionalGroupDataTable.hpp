#pragma once

#include "DataTable.hpp"
#include "FunctionalGroupData.hpp"

class FunctionalGroupDataTable :
	public DataTable<ComponentIdType, std::string, FunctionalGroupData>
{
public:
	FunctionalGroupDataTable() = default;
	FunctionalGroupDataTable(const FunctionalGroupDataTable&) = delete;

	bool loadFromFile(const std::string& path);

	size_t findFirst(const std::string& smiles) const;
};