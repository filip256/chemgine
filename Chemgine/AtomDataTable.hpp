#pragma once

#include "DataTable.hpp"
#include "AtomData.hpp"

class AtomDataTable : public DataTable<ComponentIdType, Symbol, AtomData>
{
private:
	void addPredefined();

public:
	AtomDataTable() = default;
	AtomDataTable(const AtomDataTable&) = delete;

	bool loadFromFile(const std::string& path);
};