#pragma once

#include "AtomData.hpp"
#include "AtomDataTable.hpp"

#include <string>


class DataStore
{
private:
	AtomDataTable atomsTable;

public:
	DataStore();
	DataStore(const DataStore&) = delete;

	DataStore& loadAtomsData(const std::string& path);
};