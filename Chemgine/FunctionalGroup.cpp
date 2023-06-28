#include "FunctionalGroup.hpp"
#include "Logger.hpp"

FunctionalGroup::FunctionalGroup(const std::string& smiles) :
	CompositeComponent(smiles, ComponentType::FUNCTIONAL)
{
	if (structure.isComplete())
	{
		Logger::log("Complete molecule '" + smiles + "' defined as functional group.", LogType::WARN);
	}
}