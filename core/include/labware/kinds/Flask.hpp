#pragma once

#include "labware/data/FlaskData.hpp"
#include "labware/kinds/ContainerComponent.hpp"
#include "mixtures/kinds/Reactor.hpp"

class Flask : public ContainerComponent<Reactor>
{
public:
    Flask(const LabwareId id, Atmosphere& atmosphere) noexcept;

    const FlaskData& getData() const override final;

    bool tryConnect(LabwareComponentBase& other) override final;
    void
    disconnect(const Ref<ContainerBase> dump, const LabwareComponentBase& other) override final;
};
