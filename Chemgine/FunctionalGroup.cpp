#include "FunctionalGroup.hpp"
#include "DataStore.hpp"
#include "Logger.hpp"

FunctionalGroup::FunctionalGroup(const ComponentIdType id) :
	CompositeComponent(id, ComponentType::FUNCTIONAL)
{
    if (dataStore().atoms.contains(id) == false)
    {
        Logger::log("Functional group id " + std::to_string(id) + " is undefined.", LogType::BAD);
        this->id = 0;
    }
}

const FunctionalGroupData& FunctionalGroup::data() const
{
    return dataStore().functionalGroups.at(id);
}

const BaseComponent* FunctionalGroup::getComponent(const size_t idx) const
{
    return dataStore().functionalGroups.at(id).getStructure().getComponent(idx);
}

uint8_t FunctionalGroup::getPrecedence() const
{
    return 9;
}

std::unordered_map<ComponentIdType, size_t> FunctionalGroup::getComponentCountMap() const
{
    return dataStore().backbones.at(id).getStructure().getComponentCountMap();
}