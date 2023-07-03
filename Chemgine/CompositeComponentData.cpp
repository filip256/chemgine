#include "CompositeComponentData.hpp"
#include "Logger.hpp"

CompositeComponentData::CompositeComponentData(
	const ComponentIdType id,
	const std::string& name,
	MolecularStructure&& structure
) noexcept :
	BaseComponentData(id, "", name, structure.getMolarMass(), structure.getRadicalAtomsCount()),
	structure(std::move(structure))
{}

const MolecularStructure& CompositeComponentData::getStructure() const
{
	return structure;
}

CompositeComponentData CompositeComponentData::create(
	const ComponentIdType id,
	const std::string& name,
	const std::string& smiles
)
{
	return CompositeComponentData(id, name, std::move(MolecularStructure(smiles)));
}