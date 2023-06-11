#pragma once

#include "DataTable.hpp"
#include "AtomData.hpp"

class AtomDataTable : public DataTable<AtomIdType, AtomData>
{
public:
	AtomDataTable();
	AtomDataTable(const AtomDataTable&) = delete;

	bool loadFromFile(const std::string& path);
};