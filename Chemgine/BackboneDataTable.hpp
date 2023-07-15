#pragma once

#include "DataTable.hpp"
#include "BackboneData.hpp"

class BackboneDataTable :
	public DataTable<ComponentIdType, std::string, BackboneData>
{
public:
	BackboneDataTable() = default;
	BackboneDataTable(const BackboneDataTable&) = delete;

	bool loadFromFile(const std::string& path);

	size_t findFirst(const std::string& smiles) const;
};