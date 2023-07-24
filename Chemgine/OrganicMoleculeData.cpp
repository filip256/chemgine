#include "OrganicMoleculeData.hpp"
#include "Logger.hpp"

OrganicMoleculeData::OrganicMoleculeData(
	const MoleculeIdType id,
	const std::string& name,
	const std::string& smiles
) noexcept :
	id(id),
	name(name),
	structure(smiles)
{
	if (this->structure.isComplete() == false)
	{
		Logger::log("Incomplete structure with id " + std::to_string(id) + " defined as backbone.", LogType::WARN);
	}
}

const MolecularStructure& OrganicMoleculeData::getStructure() const
{
	return structure;
}