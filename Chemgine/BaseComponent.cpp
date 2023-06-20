#include "BaseComponent.hpp"
#include "Logger.hpp"

const DataStore* BaseComponent::dataStore = nullptr;

BaseComponent::BaseComponent(const ComponentType type) :
	type(type)
{
	if (dataStore == nullptr)
		Logger::log("No datastore was set for molecular components.", LogType::FATAL);
}

void BaseComponent::setDataStore(const DataStore* const dataStore)
{
	BaseComponent::dataStore = dataStore;
}

bool BaseComponent::isCompositeType(const BaseComponent& component)
{
	return 
		component.type == ComponentType::COMPOSITE ||
		component.type == ComponentType::FUNCTIONAL ||
		component.type == ComponentType::BACKBONE;
}