#include "Backbone.hpp"
#include "DataStore.hpp"
#include "Logger.hpp"

Backbone::Backbone(const ComponentIdType id) :
    CompositeComponent(id, ComponentType::FUNCTIONAL)
{
    if (dataStore().atoms.contains(id) == false)
    {
        Logger::log("Backbone id " + std::to_string(id) + " is undefined.", LogType::BAD);
        this->id = 0;
    }
}

const BackboneData& Backbone::data() const
{
    return dataStore().backbones[id];
}

const BaseComponent* Backbone::getComponent(const size_t idx) const
{
    return dataStore().backbones[id].getStructure().getComponent(idx);
}