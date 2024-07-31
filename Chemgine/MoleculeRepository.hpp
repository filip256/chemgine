#pragma once

#include "Repository.hpp"
#include "MoleculeData.hpp"
#include "EstimatorRepository.hpp"
#include "DefinitionObject.hpp"

class MoleculeRepository :
	public Repository<MoleculeId, std::string, MoleculeData>
{
private:
	EstimatorRepository& estimators;

public:
	MoleculeRepository(EstimatorRepository& estimators) noexcept;
	MoleculeRepository(const MoleculeRepository&) = delete;

	bool add(DefinitionObject&& definition);

	bool loadFromFile(const std::string& path);
	bool saveToFile(const std::string& path);

	size_t findFirst(const MolecularStructure& structure) const;

	MoleculeId findOrAdd(MolecularStructure&& structure);
};
