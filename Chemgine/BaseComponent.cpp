#include "BaseComponent.hpp"
#include "Logger.hpp"

const DataStore* BaseComponent::dataStore = nullptr;

BaseComponent::BaseComponent()
{
	if (dataStore == nullptr)
		Logger::log("No datastore was set for molecular components.", LogType::FATAL);
}

void BaseComponent::setDataStore(const DataStore* const dataStore)
{
	BaseComponent::dataStore = dataStore;
}