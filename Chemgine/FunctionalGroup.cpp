#include "FunctionalGroup.hpp"
#include "DataStore.hpp"
#include "Logger.hpp"

FunctionalGroup::FunctionalGroup(const ComponentIdType id) :
	CompositeComponent(ComponentType::FUNCTIONAL),
    id(id)
{
    if (dataStore().atoms.contains(id) == false)
    {
        Logger::log("Atomic id " + std::to_string(id) + " is undefined.", LogType::BAD);
        this->id = 0;
    }
}

const FunctionalGroupData& FunctionalGroup::data() const
{
    return dataStore().functionalGroups[id];
}