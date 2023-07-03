#include "FunctionalGroupData.hpp"
#include "Logger.hpp"

FunctionalGroupData::FunctionalGroupData(
	const ComponentIdType id,
	const std::string& name,
	const std::string& smiles
) noexcept :
	CompositeComponentData(std::move(CompositeComponentData::create(id, name, smiles)))
{
	if (this->structure.isComplete())
	{
		Logger::log("Complete structure with id " + std::to_string(id) + " defined as functional group.", LogType::WARN);
	}
}