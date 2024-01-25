#include "CompositeComponentData.hpp"
#include "Logger.hpp"

CompositeComponentData::CompositeComponentData(
	const ComponentIdType id,
	MolecularStructure&& structure
) noexcept :
	BaseComponentData(id, structure.getMolarMass().to<Unit::GRAM>(Amount<Unit::MOLE>(1.0)), structure.getRadicalAtomsCount()),
	structure(std::move(structure))
{}

const MolecularStructure& CompositeComponentData::getStructure() const
{
	return structure;
}

uint8_t CompositeComponentData::getFittingValence(const uint8_t bonds) const
{
	return 0;
}

std::string CompositeComponentData::getSMILES() const
{
	return "[?]";
}

std::string CompositeComponentData::getBinaryId() const
{
	return "??";
}

CompositeComponentData CompositeComponentData::create(
	const ComponentIdType id,
	const std::string& smiles
)
{
	return CompositeComponentData(id, std::move(MolecularStructure(smiles)));
}