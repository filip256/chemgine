#pragma once

#include "atomics/AtomRepository.hpp"
#include "data/FileStore.hpp"
#include "data/OutlineDefRepository.hpp"
#include "estimators/EstimatorRepository.hpp"
#include "labware/LabwareRepository.hpp"
#include "molecules/MoleculeRepository.hpp"
#include "reactions/ReactionRepository.hpp"

class DataStore
{
public:
    FileStore fileStore;

    OutlineDefRepository outlineDefinitions;

    AtomRepository atoms;

    EstimatorRepository estimators;

    mutable MoleculeRepository molecules;
    ReactionRepository         reactions;

    LabwareRepository labware;

    DataStore();
    DataStore(const DataStore&) = delete;
    DataStore(DataStore&&)      = default;

    size_t totalDefinitionCount() const;

    bool addDefinition(def::Object&& definition);

    bool load(const std::string& path);
    void dump(const std::string& path, const bool prettify = true) const;
    void clear();

    static constexpr size_t npos = static_cast<size_t>(-1);
};
