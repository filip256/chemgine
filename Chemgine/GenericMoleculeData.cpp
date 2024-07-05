#include "GenericMoleculeData.hpp"
#include "Log.hpp"

GenericMoleculeData::GenericMoleculeData(
	const MoleculeId id,
	const std::string& smiles
) noexcept :
	id(id),
	structure(smiles)
{
	if (this->structure.isConcrete())
	{
		Log(this).warn("Complete structure with id {0} defined as generic molecule.", id);
	}
}

GenericMoleculeData::GenericMoleculeData(
	const MoleculeId id,
	MolecularStructure&& structure
) noexcept :
	id(id),
	structure(std::move(structure))
{
	if (this->structure.isConcrete())
	{
		Log(this).warn("Complete structure with id {0} defined as generic molecule.", id);
	}
}

const MolecularStructure& GenericMoleculeData::getStructure() const
{
	return structure;
}

std::string GenericMoleculeData::getHRTag() const
{
	return '<' + std::to_string(id) + ':' + structure.toSMILES() + '>';
}
