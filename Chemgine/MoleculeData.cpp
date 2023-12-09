#include "MoleculeData.hpp"
#include "Logger.hpp"

MoleculeData::MoleculeData(
	const MoleculeIdType id,
	const std::string& name,
	const std::string& smiles,
	const BaseApproximator& densityApproximator
) noexcept :
	id(id),
	name(name),
	structure(smiles),
	densityApproximator(densityApproximator)
{
	if (this->structure.isComplete() == false)
	{
		Logger::log("Incomplete structure with id " + std::to_string(id) + " defined as molecule.", LogType::WARN);
	}
}

MoleculeData::MoleculeData(
	const MoleculeIdType id,
	MolecularStructure&& structure,
	const BaseApproximator& densityApproximator
) noexcept :
	id(id),
	name("?"),
	structure(std::move(structure)),
	densityApproximator(densityApproximator)
{
	if (this->structure.isComplete() == false)
	{
		Logger::log("Incomplete structure with id " + std::to_string(id) + " defined as molecule.", LogType::WARN);
	}
}

const MolecularStructure& MoleculeData::getStructure() const
{
	return structure;
}