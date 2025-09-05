#pragma once

#include "data/def/Object.hpp"
#include "estimators/EstimatorRepository.hpp"
#include "molecules/data/GenericMoleculeData.hpp"
#include "molecules/data/MoleculeData.hpp"

class MoleculeRepository
{
private:
    std::unordered_map<MoleculeId, std::unique_ptr<const MoleculeData>>        concreteMolecules;
    std::unordered_map<MoleculeId, std::unique_ptr<const GenericMoleculeData>> genericMolecules;

    EstimatorRepository& estimators;

    MoleculeId getFreeId() const;

public:
    MoleculeRepository(EstimatorRepository& estimators) noexcept;
    MoleculeRepository(const MoleculeRepository&) = delete;
    MoleculeRepository(MoleculeRepository&&)      = default;

    bool add(const def::Object& definition);

    bool                contains(const MoleculeId id) const;
    const MoleculeData& at(const MoleculeId id) const;

    size_t totalDefinitionCount() const;

    const MoleculeData*        findFirstConcrete(const MolecularStructure& structure) const;
    const GenericMoleculeData* findFirstGeneric(const MolecularStructure& structure) const;

    const MoleculeData&        findOrAddConcrete(MolecularStructure&& structure);
    const GenericMoleculeData& findOrAdd(MolecularStructure&& structure);

    using Iterator = std::unordered_map<MoleculeId, std::unique_ptr<const MoleculeData>>::const_iterator;
    Iterator begin() const;
    Iterator end() const;

    size_t size() const;
    void   clear();
};
