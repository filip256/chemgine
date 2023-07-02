#include "FunctionalGroupData.hpp"
#include "Logger.hpp"

FunctionalGroupData::FunctionalGroupData(
	const ComponentIdType id,
	const std::string& name,
	MolecularStructure&& structure
) noexcept :
	BaseComponentData(id, "", name, structure.getMolarMass(), structure.getRadicalAtomsCount()),
	structure(std::move(structure))
{
	if (this->structure.isComplete())
	{
		Logger::log("Complete structure with id " + std::to_string(id) + " defined as functional group.", LogType::WARN);
	}
}

FunctionalGroupData FunctionalGroupData::create(
	const ComponentIdType id,
	const std::string& name,
	const std::string& smiles
)
{
	return FunctionalGroupData(id, name, std::move(MolecularStructure(smiles)));
}
