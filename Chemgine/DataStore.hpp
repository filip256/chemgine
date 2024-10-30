#pragma once

#include "AtomRepository.hpp"
#include "MoleculeRepository.hpp"
#include "ReactionRepository.hpp"
#include "LabwareRepository.hpp"
#include "EstimatorRepository.hpp"
#include "OOLDefRepository.hpp"
#include "FileStore.hpp"

class DataStore
{
private:
	bool addDefinition(Def::Object&& definition);

public:
	FileStore fileStore;

	OOLDefRepository oolDefinitions;

	AtomRepository atoms;

	EstimatorRepository estimators;

	mutable MoleculeRepository molecules;
	ReactionRepository reactions;

	LabwareRepository labware;

	DataStore();
	DataStore(const DataStore&) = delete;

	bool load(const std::string& path);
	void dump(const std::string& path, const bool prettify = true);
	void clear();

	static constexpr size_t npos = static_cast<size_t>(-1);
};
