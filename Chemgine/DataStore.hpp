#pragma once

#include "AtomDataTable.hpp"
#include "FunctionalGroupDataTable.hpp"

#include <string>

class DataStore
{
public:
	AtomDataTable atoms;
	FunctionalGroupDataTable functionalGroups;

	DataStore();
	DataStore(const DataStore&) = delete;

	DataStore& loadAtomsData(const std::string& path);
	DataStore& loadFunctionalGroupsData(const std::string& path);
};