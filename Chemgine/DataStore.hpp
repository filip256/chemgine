#pragma once

#include "AtomDataTable.hpp"
#include "GenericMoleculeDataTable.hpp"
#include "MoleculeDataTable.hpp"
#include "ReactionDataTable.hpp"
#include "LabwareDataTable.hpp"
#include "EstimatorDataTable.hpp"

class DataStore
{
public:
	AtomDataTable atoms;

	EstimatorDataTable estimators;

	mutable MoleculeDataTable molecules;
	mutable GenericMoleculeDataTable genericMolecules;
	ReactionDataTable reactions;

	LabwareDataTable labware;


	DataStore();
	DataStore(const DataStore&) = delete;

	DataStore& loadAtomsData(const std::string& path);

	DataStore& loadEstimatorsData(const std::string& path);
	DataStore& loadMoleculesData(const std::string& path);
	DataStore& loadGenericMoleculesData(const std::string& path);
	DataStore& loadReactionsData(const std::string& path);

	DataStore& loadLabwareData(const std::string& path);

	
	DataStore& saveMoleculesData(const std::string& path);
	DataStore& saveGenericMoleculesData(const std::string& path);
	

	static constexpr size_t npos = static_cast<size_t>(-1);
};