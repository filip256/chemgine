#pragma once

#include "Repository.hpp"
#include "MoleculeData.hpp"
#include "EstimatorRepository.hpp"
#include "DefinitionObject.hpp"

class MoleculeRepository
{
private:
	std::unordered_map<MoleculeId, std::unique_ptr<const MoleculeData>> table;

	EstimatorRepository& estimators;

	MoleculeId getFreeId() const;

public:
	MoleculeRepository(EstimatorRepository& estimators) noexcept;
	MoleculeRepository(const MoleculeRepository&) = delete;

	bool add(DefinitionObject&& definition);

	const MoleculeData& at(const MoleculeId id) const;

	const MoleculeData* findFirst(const MolecularStructure& structure) const;

	const MoleculeData& findOrAdd(MolecularStructure&& structure);

	using Iterator = std::unordered_map<MoleculeId, std::unique_ptr<const MoleculeData>>::const_iterator;
	Iterator begin() const;
	Iterator end() const;

	size_t size() const;
	void clear();
};
