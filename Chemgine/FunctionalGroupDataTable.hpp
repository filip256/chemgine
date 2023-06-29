#pragma once

#include "DataTable.hpp"
#include "FunctionalGroupData.hpp"

class FunctionalGroupDataTable :
	public DataTable<ComponentIdType, std::string, FunctionalGroupData>
{
public:
	FunctionalGroupDataTable();
	FunctionalGroupDataTable(const FunctionalGroupDataTable&) = delete;

	bool loadFromFile(const std::string& path);
};