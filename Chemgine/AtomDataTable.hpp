#pragma once

#include "DataTable.hpp"
#include "AtomData.hpp"

class AtomDataTable : public DataTable<AtomIdType, std::string, AtomData>
{
public:
	AtomDataTable();
	AtomDataTable(const AtomDataTable&) = delete;

	bool loadFromFile(const std::string& path);
};