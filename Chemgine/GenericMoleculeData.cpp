#include "GenericMoleculeData.hpp"
#include "Logger.hpp"

GenericMoleculeData::GenericMoleculeData(
	const MoleculeId id,
	const std::string& smiles
) noexcept :
	id(id),
	structure(smiles)
{
	if (this->structure.isComplete())
	{
		Logger::log("Complete structure with id " + std::to_string(id) + " defined as generic molecule.", LogType::WARN);
	}
}

GenericMoleculeData::GenericMoleculeData(
	const MoleculeId id,
	MolecularStructure&& structure
) noexcept :
	id(id),
	structure(std::move(structure))
{
	if (this->structure.isComplete())
	{
		Logger::log("Complete structure with id " + std::to_string(id) + " defined as generic molecule.", LogType::WARN);
	}
}

const MolecularStructure& GenericMoleculeData::getStructure() const
{
	return structure;
}