#include "GenericMoleculeData.hpp"
#include "Log.hpp"

GenericMoleculeData::GenericMoleculeData(
	const MoleculeId id,
	MolecularStructure&& structure
) noexcept :
	id(id),
	structure(std::move(structure))
{}

const MolecularStructure& GenericMoleculeData::getStructure() const
{
	return structure;
}
