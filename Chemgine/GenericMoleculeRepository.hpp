#pragma once

#include "Repository.hpp"
#include "GenericMoleculeData.hpp"

class GenericMoleculeRepository :
	public Repository<MoleculeId, std::string, GenericMoleculeData>
{
public:
	GenericMoleculeRepository() = default;
	GenericMoleculeRepository(const GenericMoleculeRepository&) = delete;

	bool saveToFile(const std::string& path);

	size_t findFirst(const MolecularStructure& structure) const;

	MoleculeId findOrAdd(MolecularStructure&& structure);
};
