#include "OrganicMoleculeData.hpp"
#include "Logger.hpp"

OrganicMoleculeData::OrganicMoleculeData(
	const MoleculeIdType id,
	const std::string& name,
	const std::string& smiles
) noexcept :
	structure(smiles)
{
	if (this->structure.isComplete() == false)
	{
		Logger::log("Incomplete structure with id " + std::to_string(id) + " defined as backbone.", LogType::WARN);
	}
}

void OrganicMoleculeData::categorize(const FunctionalGroupDataTable& patterns)
{
	for (size_t i = 0; i < patterns.size(); ++i)
	{
		while (true)
		{
			const auto mapping = structure.mapTo(patterns[i].getStructure());
			if (mapping.empty())
				break;

			// insert logic here
		}

	}
}