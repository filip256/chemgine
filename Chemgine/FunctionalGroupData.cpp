#include "FunctionalGroupData.hpp"

FunctionalGroupData::FunctionalGroupData(
	const ComponentIdType id,
	const std::string& smiles,
	const std::string& name,
	MolecularStructure&& structure
) :
	BaseComponentData(id, symbol, name, structure.getMolarMass(), structure.getRadicalAtomsCount()),
	structure(std::move(structure))
{
}