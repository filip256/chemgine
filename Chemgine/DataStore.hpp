#pragma once

#include "AtomRepository.hpp"
#include "GenericMoleculeRepository.hpp"
#include "MoleculeRepository.hpp"
#include "ReactionRepository.hpp"
#include "LabwareRepository.hpp"
#include "EstimatorRepository.hpp"

class DataStore
{
public:
	AtomRepository atoms;

	EstimatorRepository estimators;

	mutable MoleculeRepository molecules;
	mutable GenericMoleculeRepository genericMolecules;
	ReactionRepository reactions;

	LabwareRepository labware;


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
