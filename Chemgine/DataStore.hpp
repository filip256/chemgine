#pragma once

#include "AtomDataTable.hpp"
#include "FunctionalGroupDataTable.hpp"
#include "BackboneDataTable.hpp"
#include "MoleculeDataTable.hpp"

#include <string>

class DataStore
{
public:
	AtomDataTable atoms;
	FunctionalGroupDataTable functionalGroups;
	BackboneDataTable backbones;

	MoleculeDataTable molecules;

	DataStore();
	DataStore(const DataStore&) = delete;

	DataStore& loadAtomsData(const std::string& path);
	DataStore& loadFunctionalGroupsData(const std::string& path);
	DataStore& loadBackbonesData(const std::string& path);
	DataStore& loadMoleculesData(const std::string& path);
};