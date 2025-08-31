#include "labware/kinds/Flask.hpp"

#include "data/DataStore.hpp"

Flask::Flask(const LabwareId id, Atmosphere& atmosphere) noexcept :
    ContainerComponent(id, LabwareType::FLASK, atmosphere)
{}

const FlaskData& Flask::getData() const { return static_cast<const FlaskData&>(data); }

bool Flask::tryConnect(LabwareComponentBase& other)
{
    if (other.isContainer()) {
        this->setOverflowTarget(other.as<BaseContainerComponent&>());
        return true;
    }

    return false;
}

void Flask::disconnect(const Ref<ContainerBase> dump, const LabwareComponentBase&) { this->setOverflowTarget(dump); }
