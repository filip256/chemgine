#pragma once

#include "atomics/AtomRepository.hpp"
#include "molecules/MoleculeRepository.hpp"
#include "reactions/ReactionRepository.hpp"
#include "labware/LabwareRepository.hpp"
#include "estimators/EstimatorRepository.hpp"
#include "data/OOLDefRepository.hpp"
#include "data/FileStore.hpp"

class DataStore
{
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
	DataStore(DataStore&&) = default;

	size_t totalDefinitionCount() const;

	bool addDefinition(def::Object&& definition);

	bool load(const std::string& path);
	void dump(const std::string& path, const bool prettify = true) const;
	void clear();

	static constexpr size_t npos = static_cast<size_t>(-1);
};
