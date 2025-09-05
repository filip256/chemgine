#include "labware/kinds/Adaptor.hpp"

#include "data/DataStore.hpp"
#include "labware/kinds/Condenser.hpp"
#include "labware/kinds/Flask.hpp"

Adaptor::Adaptor(const LabwareId id, Atmosphere& atmosphere) noexcept :
    ContainerComponent(id, LabwareType::ADAPTOR, atmosphere)
{}

const AdaptorData& Adaptor::getData() const { return static_cast<const AdaptorData&>(data); }

bool Adaptor::tryConnect(LabwareComponentBase& other)
{
    auto& container = getContent<0>();
    if (other.isFlask()) {
        container.setAllIncompatibilityTargets(other.as<Flask&>().getContent());
        return true;
    }

    if (other.isAdaptor()) {
        container.setOverflowTarget(other.as<Adaptor&>().getContent());
        return true;
    }

    if (other.isCondenser()) {
        if (id != 302)
            container.setOverflowTarget(other.as<Condenser&>().getContent());
        return true;
    }

    return false;
}

void Adaptor::disconnect(const Ref<ContainerBase> dump, const LabwareComponentBase& other)
{
    auto& container = getContent<0>();
    if (other.isFlask())
        container.setAllIncompatibilityTargets(dump);

    if (other.isAdaptor())
        container.setOverflowTarget(dump);
}
