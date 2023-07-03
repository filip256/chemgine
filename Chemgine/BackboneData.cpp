#include "BackboneData.hpp"
#include "Logger.hpp"

BackboneData::BackboneData(
	const ComponentIdType id,
	const std::string& name,
	const std::string& smiles
) noexcept :
	CompositeComponentData(std::move(CompositeComponentData::create(id, name, smiles)))
{
	if (this->structure.isComplete() == false)
	{
		Logger::log("Incomplete structure with id " + std::to_string(id) + " defined as backbone.", LogType::WARN);
	}
}
