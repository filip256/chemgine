#pragma once

#include "AtomDataTable.hpp"
#include "FunctionalGroupDataTable.hpp"
#include "BackboneDataTable.hpp"
#include "MoleculeDataTable.hpp"
#include "ReactionDataTable.hpp"
#include "LabwareDataTable.hpp"
#include "ApproximatorDataTable.hpp"

class DataStore
{
public:
	AtomDataTable atoms;
	FunctionalGroupDataTable functionalGroups;
	BackboneDataTable backbones;

	mutable MoleculeDataTable molecules;
	ReactionDataTable reactions;
	ApproximatorDataTable approximators;

	LabwareDataTable labware;


	DataStore();
	DataStore(const DataStore&) = delete;

	DataStore& loadAtomsData(const std::string& path);
	DataStore& loadFunctionalGroupsData(const std::string& path);
	DataStore& loadBackbonesData(const std::string& path);

	DataStore& loadMoleculesData(const std::string& path);
	DataStore& loadReactionsData(const std::string& path);
	DataStore& loadApproximatorsData(const std::string& path);

	DataStore& loadLabwareData(const std::string& path);

	

	static constexpr size_t npos = static_cast<size_t>(-1);
};