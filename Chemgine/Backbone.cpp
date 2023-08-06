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
    return dataStore().backbones.at(id);
}

const BaseComponent* Backbone::getComponent(const c_size idx) const
{
    return dataStore().backbones.at(id).getStructure().getComponent(idx);
}

uint8_t Backbone::getPrecedence() const
{
    return 100;
}

std::unordered_map<ComponentIdType, c_size> Backbone::getComponentCountMap() const
{
    return dataStore().backbones.at(id).getStructure().getComponentCountMap();
}