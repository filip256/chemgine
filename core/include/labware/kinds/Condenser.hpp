#pragma once

#include "labware/data/CondenserData.hpp"
#include "labware/kinds/ContainerComponent.hpp"
#include "mixtures/kinds/Atmosphere.hpp"
#include "mixtures/kinds/Reactor.hpp"

class Condenser : public ContainerComponent<Atmosphere, Reactor>
{
public:
    Condenser(const LabwareId id, Atmosphere& atmosphere) noexcept;

    const CondenserData& getData() const override final;

    bool tryConnect(LabwareComponentBase& other) override final;
    void
    disconnect(const Ref<ContainerBase> dump, const LabwareComponentBase& other) override final;

    void tick(const Amount<Unit::SECOND> timespan) override final;
};
